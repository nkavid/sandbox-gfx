load(
    ":tools/clang_format/factory.bzl",
    "clang_format_attr_factory",
    "clang_format_impl_factory",
)

def _clang_format_impl(ctx):
    return [clang_format_impl_factory(ctx)]

_clang_format = rule(
    implementation = _clang_format_impl,
    attrs = clang_format_attr_factory(),
    executable = True,
)

def clang_format(**kwargs):
    #    tags = kwargs.get("tags", [])
    #    if "manual" not in tags:
    #        tags.append("manual")
    #        kwargs["tags"] = tags
    _clang_format(**kwargs)
