#pragma once

#include "ogm/ast/parse.h"
#include "ogm/common/error.hpp"
#include "ogm/common/util.hpp"

#include <map>
#include <string>
#include <cassert>

// Forward declaration if needed, but wait, `ReflectionAccumulator` is declared in `bytecode.hpp`.
// So we include `generate_utils.hpp` from `bytecode.hpp` at the very end of `bytecode.hpp` where `ReflectionAccumulator` is fully defined.
// To avoid cyclic dependency parsing issues, let's just make sure this doesn't include bytecode.hpp back if it's already included.
// Actually wait, if we remove `#include "ogm/bytecode/bytecode.hpp"`, we need `ReflectionAccumulator` to be declared.
// Since `bytecode.hpp` includes `generate_utils.hpp` *after* defining `ReflectionAccumulator`, we don't need to include `bytecode.hpp` here.

namespace ogm
{
namespace bytecode
{
class ReflectionAccumulator;

struct EnumData {
    std::map<std::string, ogm_ast_t*> m_map;

    ~EnumData()
    {
        for (const auto& pair : m_map)
        {
            delete std::get<1>(pair);
        }
    }
};

struct EnumTable {
    std::map<std::string, EnumData> m_map;
};

inline ReflectionAccumulator::ReflectionAccumulator()
    : m_namespace_instance()
    , m_bare_globals()
    , m_ast_macros()
    , m_enums(new EnumTable())
{ }

inline ReflectionAccumulator::~ReflectionAccumulator()
{
    // free macro ASTs
    for (auto& pair : m_ast_macros)
    {
        ogm_ast_free(std::get<1>(pair));
    }

    // delete enums
    assert(m_enums != nullptr);
    delete(m_enums);
}

inline void ReflectionAccumulator::set_macro(const char* name, const char* value, int flags)
{
    WRITE_LOCK(m_mutex_macros);
    auto& macros = m_ast_macros;
    ogm_ast_t* ast;
    flags |= ogm_ast_parse_flag_no_decorations;
    try
    {
        // try parsing as an expression
        ast = ogm_ast_parse_expression(value, flags);
    }
    catch (...)
    {
        // not a valid expression -- try as a statement instead
        try
        {
            ast = ogm_ast_parse(value, flags);
        }
        catch (...)
        {
            throw ogm::CompileError(ErrorCode::C::parsemacro, "Cannot parse macro \"{}\" as either an expression nor statement: \"{}\"", name, value);
        }
    }

    if (m_ast_macros.find(name) != m_ast_macros.end())
    {
        // overwrite existing macro
        ogm_ast_free(m_ast_macros[name]);
    }
    m_ast_macros[name] = ast;
}

} // namespace bytecode
} // namespace ogm