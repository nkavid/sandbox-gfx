"""Load dependencies in workspace needed to build gfx."""

load("@bazel_pkg_config//:pkg_config.bzl", "pkg_config")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

def gfx_dependencies():
    _expect("bazel_tools")
    _expect("bazel_pkg_config")
    _expect("glm")
    _expect("stb")
    _expect("rules_cuda")
    _expect("rules_foreign_cc")

    maybe(pkg_config, "libavcodec")
    maybe(pkg_config, "libavformat")
    maybe(pkg_config, "libswscale")

    maybe(pkg_config, "fmt")

    maybe(
        http_archive,
        "glew",
        build_file = "@//:third_party/glew.BUILD.bazel",
        sha256 = "a9046a913774395a095edcc0b0ac2d81c3aacca61787b39839b941e9be14e0d4",
        strip_prefix = "glew-2.2.0",
        url = "https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip",
    )

    maybe(
        new_git_repository,
        "glfw",
        build_file = "@//:third_party/glfw.BUILD.bazel",
        commit = "7482de6071d21db77a7236155da44c172a7f6c9e",
        remote = "https://github.com/glfw/glfw.git",
        shallow_since = "1657836683 +0200",
    )

def _expect(repo):
    """Temporary while stuff is moving around."""
    if not native.existing_rule(repo):
        fail(("expected '{}' repository").format(repo))
