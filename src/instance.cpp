#include "instance.h"

Instance::Instance(char* path_to_exefile_)
{
    m_path_to_exefile = path_to_exefile_;
    m_path_to_dir = m_path_to_exefile.substr(0, m_path_to_exefile.find_last_of("/\\"));
}

bool Instance::Initialize()
{
    return true;
}

bool Instance::Run()
{
    return true;
}

void Instance::Shutdown()
{

}
