#include <BMP.hpp>

namespace my::images{
    //#define MY_DEBUG_STEPS

    BMP::BMP(){
        m_width = 0;
        m_height = 0;
    }

    BMP::BMP(int width, int height){
        m_width = width;
        m_height = height;
        m_pixels = new Pixel * [m_height];
        for (int i = 0; i < m_height; i++)
            m_pixels[i] = new Pixel[m_width];

        m_coordinates = new Vec2d * [m_height];
        for (int i = 0; i < m_height; i++)
            m_coordinates[i] = new Vec2d[m_width];

        for (int i = 0; i < m_height; i++)
            for (int j = 0; j < m_width; j++)
                m_pixels[i][j] = { 0,0,0 };

        for (int i = 0; i < m_height; i++)
            for (int j = 0; j < m_width; j++){
                m_coordinates[i][j].set(0, 0, j);
                m_coordinates[i][j].set(1, 0, i);
            }
    }

    BMP::BMP(const BMP& bmp){
        if (m_pixels != nullptr){
            for (int i = 0; i < m_height; i++)
                delete[] m_pixels[i];
            delete[] m_pixels;
        }

        m_width = bmp.m_width;
        m_height = bmp.m_height;

        m_pixels = new Pixel * [m_height];
        for (int i = 0; i < m_height; i++)
            m_pixels[i] = new Pixel[m_width];

        for (int i = 0; i < m_height; i++)
            for (int j = 0; j < m_width; j++)
                m_pixels[i][j] = bmp.m_pixels[i][j];
    }

    BMP& BMP::operator=(const BMP& bmp){
        if (m_pixels != nullptr){
            for (int i = 0; i < m_height; i++)
                delete[] m_pixels[i];
            delete[] m_pixels;
        }

        m_width = bmp.m_width;
        m_height = bmp.m_height;

        m_pixels = new Pixel * [m_height];
        for (int i = 0; i < m_height; i++)
            m_pixels[i] = new Pixel[m_width];

        for (int i = 0; i < m_height; i++)
            for (int j = 0; j < m_width; j++)
                m_pixels[i][j] = bmp.m_pixels[i][j];

        return *this;
    }

    BMP::~BMP(){
        for (int i = 0; i < m_height; i++)
            delete[] m_pixels[i];
        delete[] m_pixels;

        for (int i = 0; i < m_height; i++)
            delete[] m_coordinates[i];
        delete[] m_coordinates;
    }

    void BMP::Fill(Pixel pixel){
        for (int i = 0; i < m_height; i++)
            for (int j = 0; j < m_width; j++)
                m_pixels[i][j] = pixel;
    }

    void BMP::Brightness(){
        for (int i = 0; i < m_height; i++)
            for (int j = 0; j < m_width; j++){
                if (m_pixels[i][j].b + 50 < 256)
                    m_pixels[i][j].b += 50;
                if (m_pixels[i][j].g + 50 < 256)
                    m_pixels[i][j].g += 50;
                if (m_pixels[i][j].r + 50 < 256)
                    m_pixels[i][j].r += 50;
            }
    }

    void BMP::Open(const std::string& filename){
        std::ifstream in(filename, std::ios::binary);

        BMPHEADER bmpHeader;
        in.read(reinterpret_cast<char*>(&bmpHeader), sizeof(BMPHEADER));

        BMPINFO bmpInfo;
        in.read(reinterpret_cast<char*>(&bmpInfo), sizeof(BMPINFO));

        if (m_pixels != nullptr){
            for (int i = 0; i < m_height; i++)
                delete[] m_pixels[i];
            delete[] m_pixels;
        }

        m_width = bmpInfo.Width;
        m_height = bmpInfo.Height;

        m_pixels = new Pixel * [m_height];
        for (int i = 0; i < m_height; i++)
            m_pixels[i] = new Pixel[m_width];

        for (int i = 0; i < bmpInfo.Height; i++){
            for (int j = 0; j < bmpInfo.Width; j++)
                in.read(reinterpret_cast<char*>(&m_pixels[i][j]), sizeof(Pixel));

            if ((3 * bmpInfo.Width) % 4 != 0)
                for (int j = 0; j < 4 - (3 * bmpInfo.Width) % 4; j++){
                    char c;
                    in.read(&c, 1);
                }
        }
    }

    void BMP::Save(const std::string& filename){
        if (m_width == 0 || m_height == 0)
            throw std::runtime_error("Zero height or width");

        std::ofstream out(filename, std::ios::binary);

        BMPHEADER bmpHeader_new;
        bmpHeader_new.Type = 0x4D42; //   BMP
        bmpHeader_new.Size = 14 + 40 + (3 * m_width * m_height);
        if (m_width % 4 != 0)
            bmpHeader_new.Size += (4 - (3 * m_width) % 4) * m_height;
        bmpHeader_new.OffBits = 54;
        bmpHeader_new.Reserved1 = 0;
        bmpHeader_new.Reserved2 = 0;

        out.write(reinterpret_cast<char*>(&bmpHeader_new), sizeof(BMPHEADER));

        BMPINFO bmpInfo_new;
        bmpInfo_new.BitCount = 24;
        bmpInfo_new.ClrImportant = 0;
        bmpInfo_new.ClrUsed = 0;
        bmpInfo_new.Compression = 0;
        bmpInfo_new.Height = m_height;
        bmpInfo_new.Planes = 1;
        bmpInfo_new.Size = 40;
        bmpInfo_new.SizeImage = bmpHeader_new.Size - 54;
        bmpInfo_new.Width = m_width;
        bmpInfo_new.XPelsPerMeter = 0;
        bmpInfo_new.YPelsPerMeter = 0;

        out.write(reinterpret_cast<char*>(&bmpInfo_new), sizeof(BMPINFO));

        for (int i = 0; i < bmpInfo_new.Height; i++){
            for (int j = 0; j < bmpInfo_new.Width; j++)
                out.write(reinterpret_cast<char*>(&m_pixels[i][j]), sizeof(Pixel));

            if ((3 * bmpInfo_new.Width) % 4 != 0)
                for (int j = 0; j < 4 - (3 * bmpInfo_new.Width) % 4; j++){
                    char c = 0;
                    out.write(&c, 1);
                }
        }
    }

    void BMP::Rotate(double angle){
        Vec2d T({ {
                          {(double)(m_width / 2)},
                          {(double)(m_height / 2)}
                  } });
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 1 done" << std::endl;
#endif
        for(int i = 0; i < m_height; i++)
            for (int j = 0; j < m_width; j++){
                m_coordinates[i][j] = m_coordinates[i][j] - T;
            }
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 2 done" << std::endl;
#endif
        Mat22d R({ {
                           {cos(angle), sin(angle)},
                           {-sin(angle), cos(angle)}
                   } });
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 3 done" << std::endl;
#endif
        for (int i = 0; i < m_height; i++)
            for (int j = 0; j < m_width; j++){
                m_coordinates[i][j] = R * m_coordinates[i][j];
            }

#ifdef MY_DEBUG_STEPS
        std::cerr << "step 4 done" << std::endl;
#endif

        int maxX = INT_MIN;
        int minX = INT_MAX;
        int maxY = INT_MIN;
        int minY = INT_MAX;
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 5 done" << std::endl;
#endif
        for (int i = 0; i < m_height; i++) {
            for (int j = 0; j < m_width; j++) {
                if (maxX < m_coordinates[i][j].get(0, 0))
                    maxX = m_coordinates[i][j].get(0, 0);
                if (minX > m_coordinates[i][j].get(0, 0))
                    minX = m_coordinates[i][j].get(0, 0);
                if (maxY < m_coordinates[i][j].get(1, 0))
                    maxY = m_coordinates[i][j].get(1, 0);
                if (minY > m_coordinates[i][j].get(1, 0))
                    minY = m_coordinates[i][j].get(1, 0);
            }
        }
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 6 done" << std::endl;
#endif
        maxX++;
        minX--;
        maxY++;
        minY--;

        int width = maxX - minX;
        int height = maxY - minY;

#ifdef MY_DEBUG_STEPS
        std::cerr << "step 7 done" << std::endl;
#endif
        Vec2d shift({ {
                              {(double)(width / 2)},
                              {(double)(height / 2)}
                      } });
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 8 done" << std::endl;
#endif

        for (int i = 0; i < m_height; i++)
            for (int j = 0; j < m_width; j++) {
                m_coordinates[i][j] = m_coordinates[i][j] + shift;
            }
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 9 done" << std::endl;
#endif
        Pixel** new_pixels = new Pixel * [height];
        for (int i = 0; i < height; i++)
            new_pixels[i] = new Pixel[width];
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 10 done" << std::endl;
#endif
        Vec2d** new_coordinates = new Vec2d * [height];
        for (int i = 0; i < height; i++)
            new_coordinates[i] = new Vec2d[width];
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 11 done" << std::endl;
#endif
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                new_pixels[i][j] = { 0,0,0 };
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 12 done" << std::endl;
#endif
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++){
                new_coordinates[i][j].set(0, 0, j);
                new_coordinates[i][j].set(0, 0, i);
            }
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 13 done" << std::endl;
#endif
        for (int i = 0; i < m_height; i++)
            for (int j = 0; j < m_width; j++){
                int x = (int)(m_coordinates[i][j].get(0, 0));
                int y = (int)(m_coordinates[i][j].get(1, 0));
                new_pixels[y][x] = m_pixels[i][j];
            }
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 14 done" << std::endl;
#endif
        for (int i = 0; i < m_height; i++)
            delete[] m_pixels[i];
        delete[] m_pixels;
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 15 done" << std::endl;
#endif
        for (int i = 0; i < m_height; i++)
            delete[] m_coordinates[i];
        delete[] m_coordinates;
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 16 done" << std::endl;
#endif
        m_pixels = new_pixels;
        m_coordinates = new_coordinates;
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 17 done" << std::endl;
#endif
        m_width = width;
        m_height = height;
#ifdef MY_DEBUG_STEPS
        std::cerr << "step 18 done" << std::endl;
#endif
    }

    void BMP::interpolation() {
        int count;
        for (int i = 1; i < m_height-1; i++)
            for (int j = 1; j < m_width-1; j++){
                if (m_pixels[i][j].r == 0 && m_pixels[i][j].g == 0 && m_pixels[i][j].b == 0){
                    count = 8;
                    if ((m_pixels[i + 1][j].r && m_pixels[i + 1][j].b && m_pixels[i + 1][j].g) == 0) count = count - 1;
                    if ((m_pixels[i + 1][j+1].r && m_pixels[i + 1][j+1].b && m_pixels[i + 1][j+1].g) == 0) count = count - 1;
                    if ((m_pixels[i][j+1].r && m_pixels[i][j+1].b && m_pixels[i][j+1].g) == 0) count = count - 1;
                    if ((m_pixels[i - 1][j].r && m_pixels[i - 1][j].b && m_pixels[i - 1][j].g) == 0) count = count - 1;
                    if ((m_pixels[i - 1][j-1].r && m_pixels[i - 1][j-1].b && m_pixels[i - 1][j-1].g) == 0) count = count - 1;
                    if ((m_pixels[i][j-1].r && m_pixels[i][j-1].b && m_pixels[i][j-1].g) == 0) count = count - 1;
                    if ((m_pixels[i + 1][j-1].r && m_pixels[i + 1][j-1].b && m_pixels[i + 1][j-1].g) == 0) count = count - 1;
                    if ((m_pixels[i - 1][j+1].r && m_pixels[i - 1][j+1].b && m_pixels[i - 1][j+1].g) == 0) count = count - 1;


                    m_pixels[i][j].r = (m_pixels[i + 1][j].r + m_pixels[i][j + 1].r + m_pixels[i - 1][j].r + m_pixels[i][j - 1].r + m_pixels[i + 1][j + 1].r + m_pixels[i - 1][j - 1].r + m_pixels[i + 1][j - 1].r + m_pixels[i - 1][j + 1].r) / count;
                    m_pixels[i][j].b = (m_pixels[i + 1][j].b + m_pixels[i][j + 1].b + m_pixels[i - 1][j].b + m_pixels[i][j - 1].b + m_pixels[i + 1][j + 1].b + m_pixels[i - 1][j - 1].b + m_pixels[i + 1][j - 1].b + m_pixels[i - 1][j + 1].b) / count;
                    m_pixels[i][j].g = (m_pixels[i + 1][j].g + m_pixels[i][j + 1].g + m_pixels[i - 1][j].g + m_pixels[i][j - 1].g + m_pixels[i + 1][j + 1].g + m_pixels[i - 1][j - 1].g + m_pixels[i + 1][j - 1].g + m_pixels[i - 1][j + 1].g) / count;

                }
            }
    }
}

