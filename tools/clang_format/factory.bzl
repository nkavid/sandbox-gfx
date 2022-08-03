load("@bazel_skylib//lib:shell.bzl", "shell")

def clang_format_attr_factory(test_rule = False):
    attrs = {
        "_runner": attr.label(
            default = "//:tools/clang_format/runner.bash.template",
            allow_single_file = True,
        ),
        "_logging": attr.label(
            default = "//:scripts/logging.sh",
            allow_single_file = True,
        ),
        "extensions": attr.string_list(
            allow_empty = False,
        ),
        "mode": attr.string(
            values = ["check", "fix"],
            mandatory = False,
        ),
        "directories": attr.string_list(
            allow_empty = False,
        ),
    }

    return attrs

def _get_find_args(extensions_without_wildcard):
    args = []
    args.append("-name ")
    args.append("*" + extensions_without_wildcard[0])
    for extension in extensions_without_wildcard[1:]:
        args.append("-o")
        args.append("-name ")
        args.append("*" + extension)
    return args

def clang_format_impl_factory(ctx, test_rule = False):
    workspace = ""

    out_file = ctx.actions.declare_file(ctx.label.name + ".bash")

    print(_get_find_args(ctx.attr.extensions))

    substitutions = {
        "@@WORKSPACE@@": workspace,
        "@@MODE@@": ctx.attr.mode,
        "@@EXTENSIONS@@": shell.array_literal(_get_find_args(ctx.attr.extensions)),
        "@@DIRECTORIES@@": " ".join([" " + dir for dir in ctx.attr.directories]),
    }

    ctx.actions.expand_template(
        template = ctx.file._runner,
        output = out_file,
        substitutions = substitutions,
        is_executable = True,
    )

    return DefaultInfo(
        files = depset([out_file]),
        runfiles = ctx.runfiles(files = [ctx.file._logging]),
        executable = out_file,
    )
