#include "heartbeats.hpp"
Rect findMaxContours(Mat skin, Mat frame) {
    Rect temp;
    Mat skintemp;
    skin.copyTo(skintemp);
    erode(skintemp, skintemp, Mat(), Point(-1, -1), 3);
    dilate(skintemp, skintemp, Mat(), Point(-1, -1), 3);
    int largest_area = 0;
    int largest_contour_index = 0;
        imshow("skintemp", skintemp);
    vector<vector<Point>> contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    findContours(skintemp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the images
    
    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
        double a = contourArea(contours[i], false);  //  Find the area of contour
        if (a > largest_area) {
            largest_area = a;
            largest_contour_index = i;                //Store the index of largest contour
            temp = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
        }
    }
    //imshow("skintemp", skintemp);
    temp.x = temp.x + (temp.width / 8);
//    temp.y = temp.y + (temp.height / 8);
    temp.width = temp.width / 4 * 3;
    temp.height = temp.height / 4 * 2;
//    temp.x = temp.x + (temp.width / 4);
//    temp.y = temp.y + (temp.height / 4);
//    temp.width = temp.width / 2;
//    temp.height = temp.height / 2;
    
    rectangle(frame, temp, Scalar(0, 255, 0), 1, 8, 0);
    //imshow("maxarea", black);
    return temp;
}

void heartBeat(Mat& skin,Mat& frame,Rect& temp) {
    static clock_t time;
    static bool t = true;
    static int Heartbeat = 0;
    static double red_avg = 0, pre_red_avg = 0;

    Scalar font_color = Scalar(0, 0, 255);
    CvPoint point = cvPoint(20, 20);
    double real_heartbeat = 0; //每分鐘心跳
    double red_total = 0.0, passthru = 0.0;
    for (int i = temp.y; i < temp.y + temp.height; i++) {
        for (int j = temp.x; j < temp.x + temp.width; j++) {
            if (skin.ptr<uchar>(i, j)[0]) {//if it's Skin do the process of adding red_scale for averaging
                red_total += (int)frame.ptr<uchar>(i, j)[2];
                passthru++;
            }
        }
    }

    red_avg = red_total / passthru;
    cout<<"red avg:"<<red_avg<<endl;
    if (pre_red_avg == 0)
        pre_red_avg = red_avg;
    //cout << fixed << setprecision(3);
    if (red_avg - red_thres > pre_red_avg) {
        if (!t) {
            Heartbeat++;
            time = clock();
            cout << Heartbeat / 2 << endl;
            real_heartbeat = Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60;
            //cout << "心跳:" << Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60 << "每分鐘" << endl;
            t = !t;
        }
    }
    else if (red_avg + red_thres < pre_red_avg) {
        if (t) {
            Heartbeat++;
            time = clock();
            cout << Heartbeat / 2 << endl;
            real_heartbeat = Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60;
            //cout << "心跳:" << Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60 << "每分鐘" << endl;
            t = !t;
        }
    }
    pre_red_avg = red_avg;
    char out1[100] = "HeartBeat time:", buffer[20] = "";
    sprintf(buffer, "%.2lf ", (Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60));
    strcat(out1, buffer);
    sprintf(buffer, "per minutes");
    strcat(out1, buffer);
    putText(frame, out1, point, FONT_HERSHEY_COMPLEX, 0.7, font_color);
    //imshow("src", frame);
}
