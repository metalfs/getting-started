#include <metal/stream.h>

void uppercase(mtl_stream &in, mtl_stream &out) {
    #pragma HLS INTERFACE axis port=in name=axis_input
    #pragma HLS INTERFACE axis port=out name=axis_output
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    mtl_stream_element element;
    do {
        element = in.read();

        for (int i = 0; i < sizeof(element.data); ++i)
        {
            // Select the ith byte from element
            auto current = element.data(i * 8 + 7, i * 8);

            // If current is lowercase, exchange it
            // by an uppercase letter
            if (current >= 'a' && current <= 'z') {
                element.data(i * 8 + 7, i * 8)
                  = current - ('a' - 'A');
            }
        }

        out.write(element);
    } while (!element.last);
}
