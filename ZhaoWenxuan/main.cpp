#include "han.h"

int main()
{
    cam_sign s;
    if (!inImgPath.is_open())
    {
        cout << "û���ҵ��ļ�" << endl;
    }
    //��ȡ�ļ��б����ͼƬ�ļ�·�����������������
    string temp;
    while (getline(inImgPath, temp))
    {
        s.imgList.push_back(temp);
    }
    s.angle_point();
    s.cam_mark();
    s.save();
    waitKey(0);
    return 0;
}
