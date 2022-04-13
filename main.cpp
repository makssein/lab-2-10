#include <BMP.hpp>
#include <iostream>

int main(){
    try{
        my::images::BMP test_bmp(1000, 1001);
        //test_bmp.Fill({ 0,255,0 });
        test_bmp.Open("../in.bmp");
        test_bmp.Rotate(acos(-1) / 2);
        test_bmp.Save("../out.bmp");
    }
    catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
