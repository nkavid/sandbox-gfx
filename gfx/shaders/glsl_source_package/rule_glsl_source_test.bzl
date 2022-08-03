load("@bazel_skylib//lib:unittest.bzl", "analysistest", "asserts")
load(":rule_glsl_source.bzl", "MyInfo", "glsl_source")

def _provider_contents_test_impl(ctx):
    env = analysistest.begin(ctx)
    target_under_test = analysistest.target_under_test(env)
    asserts.equals(env, "some value", target_under_test[MyInfo].val)
    return analysistest.end(env)

provider_contents_test = analysistest.make(_provider_contents_test_impl)

def _test_provider_contents():
    glsl_source(
        name = "glsl_source_subject",
        directory = "src/shaders",
        stages = ["vertex", "fragment"],
        tags = ["manual"],
    )

    provider_contents_test(
        name = "glsl_source_test",
        target_under_test = ":glsl_source_subject",
    )

def glsl_source_test_suite(name):
    _test_provider_contents()

    native.test_suite(
        name = name,
        tests = [
            ":glsl_source_test",
        ],
    )
