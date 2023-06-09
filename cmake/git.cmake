# Calls git commands to get some branch info
function(
  get_git_info
  git_branch
  git_tag_name
  git_tag_subject
  git_rev_hash
)
  execute_process(
    COMMAND git branch --show-current
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE current_branch
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  set(${git_branch}
      ${current_branch}
      PARENT_SCOPE
  )

  execute_process(
    COMMAND git tag -n1 --points-at
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE current_tag_out
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  string(
    REPLACE
      "          "
      ";"
      current_tag_list
      ${current_tag_out}
  )
  list(
    GET
    current_tag_list
    0
    current_tag_name
  )
  list(
    GET
    current_tag_list
    1
    current_tag_subject
  )
  set(${git_tag_name}
      ${current_tag_name}
      PARENT_SCOPE
  )
  set(${git_tag_subject}
      ${current_tag_subject}
      PARENT_SCOPE
  )

  execute_process(
    COMMAND git rev-parse --verify --short HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE current_rev_hash
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  set(${git_rev_hash}
      ${current_rev_hash}
      PARENT_SCOPE
  )
endfunction()
