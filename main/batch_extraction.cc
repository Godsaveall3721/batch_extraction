#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * 模拟FS树的结构体
 */
struct Node {
  std::basic_string<char> name;
  bool is_dir;
  std::vector<Node> children;
  Node(const std::basic_string<char>& n, bool dir) : name(n), is_dir(dir) {}
};

/**
 * 执行系统命令并返回输出结果
 * @param cmd 传入Linux命令字符串
 * @return 命令处理后的输出结果
 */
std::basic_string<char> exec(const std::basic_string<char>& cmd) {
  std::array<char, 128> rebuffer;
  std::basic_string<char> result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"),
                                                pclose);
  if (!pipe) {
    throw std::runtime_error("exec函数中命令行通道打开失败!");
  }
  while (fgets(rebuffer.data(), rebuffer.size(), pipe.get()) != nullptr) {
    result += rebuffer.data();
  }
  return result;
}

/**
 * 保证目录树处于根
 */
void ensure_in_root() {
  long i = 0;
  while (true) {
    i++;
    std::basic_string<char> result2 =
        exec("hcd :: 2>&1");  // 2>&1 stderr重定向到stdout
    if (result2 == "hcd: \"::\": no such file or directory\n") {
      break;
    }
    if (i >= 30) {
      throw std::runtime_error("目录深度超过30了");
    }
  }
};

/**
 * 封装 hls 列出当前目录内容的命令
 * @return
 */
std::vector<std::string> hls() {
  std::vector<std::string> entries;
  std::basic_string<char> result = exec("hls 2>&1");  // 调用 hls 命令
  size_t pos = 0;
  while ((pos = result.find("\n")) != std::string::npos) {
    entries.push_back(result.substr(0, pos));
    result.erase(0, pos + 1);
  }
  return entries;
}

/**
 * 封装 hcd 进入目录命令
 * @param dir 当前目录下存在的子目录
 * @return 返回0则不成功 返回1则成功
 */
bool hcd(const std::basic_string<char>& dir) {
  std::basic_string<char> cmd = "hcd \"" + dir + "\" 2>&1";
  return exec(cmd) == "";
}

/**
 * 遍历当前目录并构建目录树
 * @param current 为了递归而传入的目录树
 */
void explore_helper(Node& current) {
  auto entries = hls();  // 获取当前目录下的所有条目

  for (const auto& name : entries) {
    if (hcd(name)) {
      // 进入子目录
      Node dir_node(name, true);
      explore_helper(dir_node);  // 递归探索
      current.children.push_back(dir_node);
      hcd("::");  // 返回上级目录
    } else {
      // 如果不是目录，那么就是文件
      Node file_node(name, false);
      current.children.push_back(file_node);
    }
  }
}

/**
 * 开始从根目录进行探索
 * @return 构建好的目录树
 */
Node explore() {
  ensure_in_root();
  // 标记根目录
  Node root("/", true);
  explore_helper(root);  // 递归探索目录
  return root;
}

/**
 * 打印目录树
 * @param node 构建好的目录树
 * @param indent 为递归准备
 */
void print_tree(const Node& node, const std::basic_string<char>& indent = "") {
  std::cout << indent << (node.is_dir ? "[DIR] " : "[FILE] ") << node.name
            << std::endl;
  for (const auto& child : node.children) {
    print_tree(child, indent + "  ");  // 增加缩进显示子节点
  }
}

/**
 * 利用递归来导出文件的同时保持FS树结构
 * @param current 构建好的目录树
 * @param path 为递归准备的路径
 */
void export_data_helper(const Node& current,
                        const std::basic_string<char>& path) {
  // 构造真实路径
  std::basic_string<char> real_path = "/home/x-pc/temp";
  if (!path.empty()) {
    real_path += "/";
    real_path += path;
  }

  // 创建当前目录（如果 path 为空，是根目录，会创建 /home/x-pc/temp）
  {
    std::string mkdir_cmd = "mkdir -p \"" + real_path + "\"";
    system(mkdir_cmd.c_str());
  }

  // 导出当前目录的文件
  for (const Node& child : current.children) {
    if (!child.is_dir) {
      // 替换LinuxFS不可用的字符'/'
      std::basic_string<char> safe_name = child.name;
      for (auto& c : safe_name) {
        if (c == '/') c = '_';
      }

      // 构造真实文件路径
      std::basic_string<char> dst_path = real_path + "/" + safe_name;

      // hcopy 保留虚拟名字
      std::basic_string<char> cmd =
          "hcopy \"" + child.name + "\" \"" + dst_path + "\"";
      exec(cmd);
    }
  }

  // 递归处理子目录
  for (const auto& child : current.children) {
    if (child.is_dir) {
      // 尝试进入经典 MacFS 子目录
      if (hcd(child.name)) {
        // 构建新的 path（真实路径部分）
        std::basic_string<char> next_path = path;

        if (!next_path.empty()) next_path += "/";

        // 同样对子目录名做安全化
        std::basic_string<char> safe_name = child.name;
        for (auto& c : safe_name) {
          if (c == '/') c = '_';
        }

        next_path += safe_name;

        // 递归导出
        export_data_helper(child, next_path);

        // 返回虚拟 FS 上一级
        hcd("::");
      } else {
        throw std::runtime_error("这个是个目录但是进不去: " + child.name);
      }
    }
  }
}

/**
 * 预处理确保处于根目录
 * @param root 构建好的目录树
 */
void export_data(const Node& root) {
  ensure_in_root();
  export_data_helper(root, "");
}

int main() {
  Node root = explore();
  print_tree(root);
  export_data(root);
  return 0;
}
