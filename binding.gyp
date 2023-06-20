{
  "variables": {
  },
  "targets": [
    {
      "target_name": "hasp",
      "sources": [ "src/main.cpp", "src/node_hasp.cpp", "src/hasp.cpp" ],
      "conditions": [
        ["OS=='linux'", {
          "cflags": [
            "-std=c++11"
          ],
          "include_dirs": [
            "libs/hasp/linux"
          ],
          "libraries": [
            "-L../libs/hasp/linux/<(target_arch)",
            "-lhasp_linux_<(target_arch)_demo",
            "-lhasp_cpp_linux_<(target_arch)",
            "-lcrypto"
          ],
        }],
      ],
    }
  ]
}
