#include "instance.h"

int main(int argc, char** argv)
{
    Instance* instance_ptr = new Instance(argv[0]);

    if(instance_ptr->Initialize())
        instance_ptr->Run();
    else
        instance_ptr->Shutdown();

    delete instance_ptr;
    instance_ptr = nullptr;

    return 0;
}
