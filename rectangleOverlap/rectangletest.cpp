#include <iostream>
#include <vector>

using namespace std;

typedef pair<double, double> MyArrow;

class MyPoint
{
public:
    double x;
    double y;

    MyPoint(){};

    MyPoint(double xx, double yy)
    {
        x = xx;
        y = yy;
    }

    MyArrow arrow(const MyPoint &another);
};

MyArrow MyPoint::arrow(const MyPoint &another)
{
    MyArrow p = make_pair(x - another.x, y - another.y);
    return p;
}

typedef pair<MyPoint, MyPoint> MySegment;

class MyRectangle
{
public:
    MyPoint point_1;
    MyPoint point_2;
    MyPoint point_3;
    MyPoint point_4;

    MyRectangle(MyPoint p1, MyPoint p2, MyPoint p3, MyPoint p4)
    {
        point_1 = p1;
        point_2 = p2;
        point_3 = p3;
        point_4 = p4;
    }

    bool isRealRectangle();
    MySegment get_1st_segment();
    MySegment get_2nd_segment();
    MySegment get_3rd_segment();
    MySegment get_4th_segment();
};

bool MyRectangle::isRealRectangle()
{
    MyArrow arr1, arr2, arr3;
    arr1 = point_1.arrow(point_2);
    arr2 = point_3.arrow(point_4);

    if (arr1.first != arr2.first || arr1.second != arr2.second)
        return false;
    else
    {
        arr3 = point_1.arrow(point_3);
        if (arr1.first * arr3.first + arr1.second * arr3.second == 0)
        {
            return true;
        }
    }
    return false;
}

MySegment MyRectangle::get_1st_segment()
{
    return make_pair(point_1, point_2);
}

MySegment MyRectangle::get_2nd_segment()
{
    return make_pair(point_2, point_3);
}

MySegment MyRectangle::get_3rd_segment()
{
    return make_pair(point_3, point_4);
}

MySegment MyRectangle::get_4th_segment()
{
    return make_pair(point_4, point_1);
}
int main()
{

    MyPoint mp1(1, 1), mp2(4, 0), mp3(3, 7), mp4(6, 6);

    MyRectangle mrec(mp1, mp2, mp3, mp4);
    printf("p1 is (%f, %f)\n", mrec.point_1.x, mrec.point_1.y);
    printf("p2 is (%f, %f)\n", mrec.point_2.x, mrec.point_2.y);
    printf("p3 is (%f, %f)\n", mrec.point_3.x, mrec.point_3.y);
    printf("p4 is (%f, %f)\n", mrec.point_4.x, mrec.point_4.y);

    if(mrec.isRealRectangle()) {
        cout << "Mrec is a real rectangle!" << endl;
    } else {
        cout << "Mrec isn't a real rectangle!" << endl;
    }
    
    MySegment ms1, ms2, ms3, ms4;

    ms1 = mrec.get_1st_segment();
    ms2 = mrec.get_2nd_segment();
    ms3 = mrec.get_3rd_segment();
    ms4 = mrec.get_4th_segment();

    cout<<"ms1 is ("<<ms1.first.x<<" ,"<<ms1.first.y<<") to ("<<ms1.second.x<<" ,"<<ms1.second.y<<")."<<endl;
    return 0;
}
