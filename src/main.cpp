#include "instance.h"

int main(int argc, char** argv)
{
    Instance* instance_ptr = new Instance();

    if(instance_ptr->Initialize(argv[0]))
        instance_ptr->Run();

    instance_ptr->Shutdown();
    delete instance_ptr;
    instance_ptr = nullptr;

    return 0;
}
