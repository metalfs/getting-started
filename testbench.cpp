#include <stdio.h>
#include <string.h>
#include <metal/stream.h>

void uppercase(mtl_stream &in, mtl_stream &out);

void copyBufferToStream(const char *buffer, size_t buffer_length, mtl_stream &stream) {
  size_t readBytes = 0;
  mtl_stream_element inputElement;
  do {
    memcpy(&inputElement.data, buffer + readBytes,
        std::min(buffer_length - readBytes, sizeof(inputElement.data)));
    inputElement.keep = 0xff;
    inputElement.last = readBytes + sizeof(inputElement.data) >= buffer_length;

    stream.write(inputElement);

    readBytes += sizeof(inputElement.data);
  } while (!inputElement.last);
}

void copyStreamToBuffer(mtl_stream &stream, char *buffer, size_t buffer_length) {
  size_t writtenBytes = 0;
  mtl_stream_element outputElement;
  do {
    outputElement = stream.read();
    memcpy(buffer + writtenBytes, &outputElement.data,
        std::min(buffer_length - writtenBytes, sizeof(outputElement.data)));

    writtenBytes += sizeof(outputElement.data);
  } while (!outputElement.last);
}

int main() {
  const char input[] = "This should become uppercase";

  // Transform our input data into a mtl_stream
  mtl_stream operatorInput;
  copyBufferToStream(input, sizeof(input), operatorInput);

  // Call the operator
  mtl_stream operatorOutput;
  uppercase(operatorInput, operatorOutput);

  // Read the output back into a buffer for comparison
  char outputData[sizeof(input)];
  copyStreamToBuffer(operatorOutput, outputData, sizeof(outputData));

  const char expected[] = "THIS SHOULD BECOME UPPERCASE";

  int result = memcmp(outputData, expected, sizeof(expected));
  if (result == 0) {
    printf("Success.\n");
  } else {
    printf("Failure: Output was different from expected value.\n");
  }


  return result;
}
