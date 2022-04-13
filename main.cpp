#include <BMP.hpp>
#include <iostream>

int main()
{
    try
    {
        mt::images::BMP test_bmp(200, 150);
        test_bmp.Fill({ 0,255,0 });
        test_bmp.Rotate(acos(-1) / 4);
        test_bmp.Save("../test.bmp");
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
