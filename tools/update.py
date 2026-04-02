import re

with open('src/interpreter/display/Display.cpp', 'r') as f:
    text = f.read()

start_idx = text.find('void Display::draw_image_tiled(')
end_idx = text.find('void Display::draw_image(', start_idx)

func_part = text[start_idx:end_idx]

search_text = r"""        const size_t vertex_count = \(offset_maxx - offset_minx\) \* \(offset_maxy - offset_miny\) \* 6;
        float\* const vertices_o = new float\[vertex_count \* k_vertex_data_size\];
        float\* vertices = vertices_o;
        for \(int32_t i = offset_minx; i < offset_maxx; \+\+i\)
        \{
            for \(int32_t j = offset_miny; j < offset_maxy; \+\+j\)
            \{
                auto p = offset_to_world\(i, j\);
                floats3pm\(vertices \+ 0\*k_vertex_data_size, p.x, p.y\);
                floats3pm\(vertices \+ 1\*k_vertex_data_size, p.x, p.y \+ height\);
                floats3pm\(vertices \+ 2\*k_vertex_data_size, p.x \+ width, p.y\);
                floats3pm\(vertices \+ 3\*k_vertex_data_size, p.x \+ width, p.y\);
                floats3pm\(vertices \+ 4\*k_vertex_data_size, p.x, p.y \+ height\);
                floats3pm\(vertices \+ 5\*k_vertex_data_size, p.x \+ width, p.y \+ height\);

                colour4_to_floats\(vertices \+ 0\*k_vertex_data_size \+ 3, g_draw_colour\[0\]\);
                colour4_to_floats\(vertices \+ 1\*k_vertex_data_size \+ 3, g_draw_colour\[1\]\);
                colour4_to_floats\(vertices \+ 2\*k_vertex_data_size \+ 3, g_draw_colour\[2\]\);
                colour4_to_floats\(vertices \+ 3\*k_vertex_data_size \+ 3, g_draw_colour\[2\]\);
                colour4_to_floats\(vertices \+ 4\*k_vertex_data_size \+ 3, g_draw_colour\[1\]\);
                colour4_to_floats\(vertices \+ 5\*k_vertex_data_size \+ 3, g_draw_colour\[3\]\);

                vertices\[0\*k_vertex_data_size \+ 7\] = texture->u_global\(tx1\);
                vertices\[0\*k_vertex_data_size \+ 8\] = texture->v_global\(ty1\);

                vertices\[1\*k_vertex_data_size \+ 7\] = texture->u_global\(tx1\);
                vertices\[1\*k_vertex_data_size \+ 8\] = texture->v_global\(ty2\);

                vertices\[2\*k_vertex_data_size \+ 7\] = texture->u_global\(tx2\);
                vertices\[2\*k_vertex_data_size \+ 8\] = texture->v_global\(ty1\);

                vertices\[3\*k_vertex_data_size \+ 7\] = texture->u_global\(tx2\);
                vertices\[3\*k_vertex_data_size \+ 8\] = texture->v_global\(ty1\);

                vertices\[4\*k_vertex_data_size \+ 7\] = texture->u_global\(tx1\);
                vertices\[4\*k_vertex_data_size \+ 8\] = texture->v_global\(ty2\);

                vertices\[5\*k_vertex_data_size \+ 7\] = texture->u_global\(tx2\);
                vertices\[5\*k_vertex_data_size \+ 8\] = texture->v_global\(ty2\);

                // TODO: switch to trianglestrip for more efficient vertex packing.
                vertices \+= k_vertex_data_size \* 6;
            \}
        \}

        render_vertices\(vertices_o, vertex_count, texture->m_tpage->m_gl_tex, GL_TRIANGLES\);
        delete\[\] vertices_o;"""

replace_text = """        for (int32_t i = offset_minx; i < offset_maxx; ++i)
        {
            for (int32_t j = offset_miny; j < offset_maxy; ++j)
            {
                float vertices[k_vertex_data_size * 4];
                auto p = offset_to_world(i, j);

                // Vertex 0: Top-Left
                floats3pm(vertices + 0*k_vertex_data_size, p.x, p.y);
                // Vertex 1: Bottom-Left
                floats3pm(vertices + 1*k_vertex_data_size, p.x, p.y + height);
                // Vertex 2: Top-Right
                floats3pm(vertices + 2*k_vertex_data_size, p.x + width, p.y);
                // Vertex 3: Bottom-Right
                floats3pm(vertices + 3*k_vertex_data_size, p.x + width, p.y + height);

                colour4_to_floats(vertices + 0*k_vertex_data_size + 3, g_draw_colour[0]);
                colour4_to_floats(vertices + 1*k_vertex_data_size + 3, g_draw_colour[1]);
                colour4_to_floats(vertices + 2*k_vertex_data_size + 3, g_draw_colour[2]);
                colour4_to_floats(vertices + 3*k_vertex_data_size + 3, g_draw_colour[3]);

                // Vertex 0: Top-Left UV
                vertices[0*k_vertex_data_size + 7] = texture->u_global(tx1);
                vertices[0*k_vertex_data_size + 8] = texture->v_global(ty1);

                // Vertex 1: Bottom-Left UV
                vertices[1*k_vertex_data_size + 7] = texture->u_global(tx1);
                vertices[1*k_vertex_data_size + 8] = texture->v_global(ty2);

                // Vertex 2: Top-Right UV
                vertices[2*k_vertex_data_size + 7] = texture->u_global(tx2);
                vertices[2*k_vertex_data_size + 8] = texture->v_global(ty1);

                // Vertex 3: Bottom-Right UV
                vertices[3*k_vertex_data_size + 7] = texture->u_global(tx2);
                vertices[3*k_vertex_data_size + 8] = texture->v_global(ty2);

                render_vertices(vertices, 4, texture->m_tpage->m_gl_tex, GL_TRIANGLE_STRIP);
            }
        }"""

new_func_part = re.sub(search_text, replace_text, func_part)

if new_func_part != func_part:
    text = text[:start_idx] + new_func_part + text[end_idx:]
    with open('src/interpreter/display/Display.cpp', 'w') as f:
        f.write(text)
    print("Replaced successfully")
else:
    print("Search text not found in function")
