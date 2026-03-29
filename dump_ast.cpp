
#include "ogm/ast/parse.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::string json_escape(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\b') out += "\\b";
        else if (c == '\f') out += "\\f";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else if ((unsigned char)c < 0x20) {
            char buf[8];
            sprintf(buf, "\\u%04x", (unsigned char)c);
            out += buf;
        } else out += c;
    }
    return out;
}

void dump_ast_json(const ogm_ast_t* tree, std::ostream& out, int indent = 0) {
    std::string ind(indent * 2, ' ');
    out << ind << "{\n";
    
    out << ind << "  \"type\": " << tree->m_type << ",\n";
    out << ind << "  \"subtype\": \"" << ogm_ast_subtype_string[tree->m_subtype] << "\",\n";
    
    payload_type_t pt = ogm_ast_tree_get_payload_type(tree);
    if (pt == ogm_ast_payload_t_spec) {
        ogm_ast_spec_t spec;
        if (ogm_ast_tree_get_spec(tree, &spec)) {
            out << ind << "  \"spec\": \"" << ogm_ast_spec_string[spec] << "\",\n";
        }
    } else if (pt == ogm_ast_payload_t_string) {
        const char* s = ogm_ast_tree_get_payload_string(tree);
        if (s) out << ind << "  \"value\": \"" << json_escape(s) << "\",\n";
    } else if (pt == ogm_ast_payload_t_literal_primitive) {
        ogm_ast_literal_primitive_t* payload;
        if (ogm_ast_tree_get_payload_literal_primitive(tree, &payload)) {
            out << ind << "  \"value\": \"" << json_escape(payload->m_value) << "\",\n";
            out << ind << "  \"literal_type\": " << payload->m_type << ",\n";
        }
    } else if (pt == ogm_ast_payload_t_declaration) {
        ogm_ast_declaration_t* payload;
        if (ogm_ast_tree_get_payload_declaration(tree, &payload)) {
            out << ind << "  \"identifiers\": [\n";
            for (int32_t i = 0; i < payload->m_identifier_count; i++) {
                out << ind << "    \"" << json_escape(payload->m_identifier[i]) << "\"";
                if (i < payload->m_identifier_count - 1) out << ",";
                out << "\n";
            }
            out << ind << "  ],\n";
        }
    }
    
    out << ind << "  \"sub\": [\n";
    for (int32_t i = 0; i < tree->m_sub_count; i++) {
        dump_ast_json(tree->m_sub + i, out, indent + 2);
        if (i < tree->m_sub_count - 1) out << ",";
        out << "\n";
    }
    out << ind << "  ]\n";
    out << ind << "}";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: dump_ast <input.gml>" << std::endl;
        return 1;
    }
    
    std::ifstream ifs(argv[1]);
    if (!ifs) {
        std::cerr << "Could not open " << argv[1] << std::endl;
        return 1;
    }
    
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    
    try {
        ogm_ast_t* ast = ogm_ast_parse(content.c_str());
        if (!ast) {
            std::cerr << "Parse failed" << std::endl;
            return 1;
        }
        dump_ast_json(ast, std::cout);
        std::cout << std::endl;
        ogm_ast_free(ast);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
