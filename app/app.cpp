#include <iostream>

#include <metal-pipeline/operator_factory.hpp>
#include <metal-pipeline/pipeline.hpp>
#include <metal-pipeline/snap_action.hpp>

using namespace metal;

int main(int, char *[])
{
    SnapAction fpga;
    auto factory = OperatorFactory::fromFPGA(fpga);

    std::string input = "Hello World";
    auto outputBuffer = SnapAction::allocateMemory(input.size());

    auto uppercase = factory.createOperator("uppercase");

    Pipeline pipeline(std::move(uppercase));
    pipeline.run(DataSource(input.c_str(), input.size()), DataSink(outputBuffer, input.size()), fpga);

    std::cout << std::string(reinterpret_cast<const char*>(outputBuffer)) << std::endl;

    return 0;
}
