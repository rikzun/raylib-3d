#define export extern "C" __declspec(dllexport)

export int multiply(int value)
{
    return value * 2;
}