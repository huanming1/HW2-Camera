#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2\imgproc\types_c.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;
using namespace cv;

class cam_sign
{
public:
    cam_sign(){}
    void angle_point();
    void cam_mark();
    void save();
public:
    vector<string> imgList;//���궨ͼƬ�б�
    Size image_size;//����ͼƬ��С
    Size pattern_size = Size(4, 6);//�궨����ÿ�С�ÿ�еĽǵ���������ͼƬ�еı궨�����ڽǵ���Ϊ4*6
    vector<vector<Point2f>> corner_points_of_all_imgs;
    int image_num = 0;
    Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));//����ξ���H������Ӧ�Ծ���
    Mat distCoefficients = Mat(1, 5, CV_32FC1, Scalar::all(0));//�������5������ϵ����k1,k2,p1,p2,k3
};

ifstream inImgPath("calibdata.txt");    //�궨����ͼ���ļ���·��
ofstream fout("caliberation_result.txt");   //����궨������ļ�

void cam_sign::angle_point()
{
    cout << "��ʼ��ȡ�ǵ�......" << endl;
    vector<Point2f> corner_points_buf;//��һ�����黺���⵽�Ľǵ㣬ͨ������Point2f��ʽ
    string filename;
    while (image_num < imgList.size())
    {
        filename = imgList[image_num++];
        cout << "image_num = " << image_num << endl;
        cout << filename.c_str() << endl;
        Mat imageInput = imread(filename.c_str());
        if (image_num == 1)
        {
            image_size.width = imageInput.cols;
            image_size.height = imageInput.rows;
            cout << "image_size.width = " << image_size.width << endl;
            cout << "image_size.height = " << image_size.height << endl;
        }

        if (findChessboardCorners(imageInput, pattern_size, corner_points_buf) == 0)
        {
            cout << "can not find chessboard corners!\n";   //�Ҳ����ǵ�
            exit(1);
        }
        else
        {
            Mat gray;
            cvtColor(imageInput, gray, CV_RGB2GRAY);
            //�����ؾ�ȷ��
            find4QuadCornerSubpix(gray, corner_points_buf, Size(5, 5));
            corner_points_of_all_imgs.push_back(corner_points_buf);
            drawChessboardCorners(gray, pattern_size, corner_points_buf, true);//��ǽǵ�
            imshow("camera calibration", gray);
            waitKey(100);
        }
    }

    int total = corner_points_of_all_imgs.size();
    cout << "total=" << total << endl;
    int cornerNum = pattern_size.width * pattern_size.height;//ÿ��ͼƬ�ϵ��ܵĽǵ���
    for (int i = 0; i < total; i++)
    {
        cout << "--> ��" << i + 1 << "��ͼƬ������ -->:" << endl;
        for (int j = 0; j < cornerNum; j++)
        {
            cout << "-->" << corner_points_of_all_imgs[i][j].x;
            cout << "-->" << corner_points_of_all_imgs[i][j].y;
            if ((j + 1) % 3 == 0)
            {
                cout << endl;
            }
            else
            {
                cout.width(10);
            }
        }
        cout << endl;
    }

    cout << endl << "�ǵ���ȡ���" << endl;
}

void cam_sign::cam_mark()
{
    //������궨
    cout << "��ʼ�궨������������" << endl;
    vector<Mat> tvecsMat;//ÿ��ͼ���ƽ��������t
    vector<Mat> rvecsMat;//ÿ��ͼ�����ת�������޵������ת������
    vector<vector<Point3f>> objectPoints;//��������ͼƬ�Ľǵ����ά����
                                             //��ʼ��ÿһ��ͼƬ�б궨���Ͻǵ����ά����
    int i, j, k;
    for (k = 0; k < image_num; k++)//����ÿһ��ͼƬ
    {
        vector<Point3f> tempCornerPoints;//ÿһ��ͼƬ��Ӧ�Ľǵ�����
        //�������еĽǵ�
        for (i = 0; i < pattern_size.height; i++)
        {
            for (j = 0; j < pattern_size.width; j++)
            {
                Point3f singleRealPoint;//һ���ǵ������
                singleRealPoint.x = i * 10;
                singleRealPoint.y = j * 10;
                singleRealPoint.z = 0;//����z=0
                tempCornerPoints.push_back(singleRealPoint);
            }
        }
        objectPoints.push_back(tempCornerPoints);
    }

    calibrateCamera(objectPoints, corner_points_of_all_imgs, image_size, cameraMatrix, distCoefficients, rvecsMat, tvecsMat, 0);
    cout << "�궨���" << endl;
    //��ʼ����궨���
    cout << "��ʼ����궨���" << endl;
    cout << endl << "�����ز�����" << endl;
    fout << "�����ز�����" << endl;
    cout << "1.�����������:" << endl;
    fout << "1.�����������:" << endl;
    cout << "��С��" << cameraMatrix.size() << endl;
    fout << "��С��" << cameraMatrix.size() << endl;
    cout << cameraMatrix << endl;
    fout << cameraMatrix << endl;

    cout << "2.����ϵ����" << endl;
    fout << "2.����ϵ����" << endl;
    cout << "��С��" << distCoefficients.size() << endl;
    fout << "��С��" << distCoefficients.size() << endl;
    cout << distCoefficients << endl;
    fout << distCoefficients << endl;

    cout << endl << "ͼ����ز�����" << endl;
    fout << endl << "ͼ����ز�����" << endl;
    Mat rotation_Matrix = Mat(3, 3, CV_32FC1, Scalar::all(0));//��ת����
    for (i = 0; i < image_num; i++)
    {
        cout << "��" << i + 1 << "��ͼ�����ת������" << endl;
        fout << "��" << i + 1 << "��ͼ�����ת������" << endl;
        cout << rvecsMat[i] << endl;
        fout << rvecsMat[i] << endl;
        cout << "��" << i + 1 << "��ͼ�����ת����" << endl;
        fout << "��" << i + 1 << "��ͼ�����ת����" << endl;
        Rodrigues(rvecsMat[i], rotation_Matrix);//����ת����ת��Ϊ���Ӧ����ת����
        cout << rotation_Matrix << endl;
        fout << rotation_Matrix << endl;
        cout << "��" << i + 1 << "��ͼ���ƽ��������" << endl;
        fout << "��" << i + 1 << "��ͼ���ƽ��������" << endl;
        cout << tvecsMat[i] << endl;
        fout << tvecsMat[i] << endl;
    }
    cout << "����������" << endl;
}
void cam_sign::save()
{
    Mat mapx = Mat(image_size, CV_32FC1);
    Mat mapy = Mat(image_size, CV_32FC1);
    Mat R = Mat::eye(3, 3, CV_32F);
    cout << "�������ͼ��" << endl;
    string imageFileName;
    std::stringstream StrStm;
    for (int i = 0; i < image_num; i++)
    {
        cout << "Frame #" << i + 1 << endl;
        initUndistortRectifyMap(cameraMatrix, distCoefficients, R, cameraMatrix, image_size, CV_32FC1, mapx, mapy);
        Mat src_image = imread(imgList[i].c_str(), 1);
        Mat new_image = src_image.clone();
        remap(src_image, new_image, mapx, mapy, INTER_LINEAR);
        imshow("ԭʼͼ��", src_image);
        imshow("������ͼ��", new_image);

        StrStm.clear();
        imageFileName.clear();
        StrStm << i + 1;
        StrStm >> imageFileName;
        imageFileName += "_d.jpg";
        imwrite(imageFileName, new_image);

        waitKey(200);
    }
    cout << "�������" << endl;
}
