#include "colorfilter.h"

#define HEADER_BYTES 138
#define HEADER_ELEMENTS ((HEADER_BYTES / STREAM_BYTES) + 1)
#define HEADER_OFFSET (STREAM_BYTES - (HEADER_BYTES % STREAM_BYTES))

#define RED_FACTOR 54
#define GREEN_FACTOR 183
#define BLUE_FACTOR 18

using ap_uint8  = ap_uint<8>;
using ap_uint16 = ap_uint<16>;
using ap_uint32 = ap_uint<32>;

ap_uint8 grayscale(ap_uint8 red, ap_uint8 green, ap_uint8 blue) {
  ap_uint16 red_contrib   = ((ap_uint16)red)   * RED_FACTOR;
  ap_uint16 green_contrib = ((ap_uint16)green) * GREEN_FACTOR;
  ap_uint16 blue_contrib  = ((ap_uint16)blue)  * BLUE_FACTOR;
  return (uint8_t)(red_contrib   >> 8) +
         (uint8_t)(green_contrib >> 8) +
         (uint8_t)(blue_contrib  >> 8);
}

ap_uint32 transform_pixel(ap_uint32 pixel) {
  ap_uint8 alpha = (ap_uint8)(pixel >> 24);
  ap_uint8 red   = (ap_uint8)(pixel >> 16);
  ap_uint8 green = (ap_uint8)(pixel >> 8);
  ap_uint8 blue  = (ap_uint8) pixel;

  ap_uint8 gray = grayscale(red, green, blue);

  if (red < green || red < blue) {
    return (alpha, gray, gray, gray);
  } else {
    return (alpha, red, green, blue);
  }
}

void process_stream(mtl_stream &in, mtl_stream &out) {
  mtl_stream_element input, output;
  ap_uint32 element_idx = 0;

  do {
    #pragma HLS PIPELINE
    input = in.read();

    if (element_idx < HEADER_ELEMENTS) {
      output = input;
      ++element_idx;
    } else {
      for (int i = 0; i < STREAM_BYTES / 4; ++i) {
        #pragma HLS UNROLL
        ap_uint32 pixel_in = input.data(31 + 32*i, 32*i);
        ap_uint32 pixel_out = transform_pixel(pixel_in);
        output.data(31 + 32*i, 32*i) = pixel_out;
      }
      output.last = input.last;
      output.keep = input.keep;
    }

    out.write(output);
  } while (!output.last);
}

void colorfilter(mtl_stream &in, mtl_stream &out) {

    #pragma HLS INTERFACE axis port=in name=axis_input
    #pragma HLS INTERFACE axis port=out name=axis_output
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    #pragma HLS DATAFLOW

    mtl_stream padded_stream;
    mtl_stream processed_stream;

    insert_padding<HEADER_OFFSET>(in, padded_stream);
    process_stream(padded_stream, processed_stream);
    remove_padding<HEADER_OFFSET>(processed_stream, out);
}
