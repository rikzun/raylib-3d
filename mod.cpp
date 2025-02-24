#define export extern "C" __declspec(dllexport)

export bool modfunc()
{
    return true;
}