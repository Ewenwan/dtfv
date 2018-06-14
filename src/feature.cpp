/*
输出idt feature的结构：

  特征是一个接一个计算的每一个都是单独一列，由下面的格式给出：
  frameNum mean_x mean_y var_x var_y length scale x_pos y_pos t_pos Trajectory HOG HOF MBHx MBHy

  前十个部分是关于轨迹的：
  frameNum:     The trajectory ends on which frame
  mean_x:       The mean value of the x coordinates of the trajectory
  mean_y:       The mean value of the y coordinates of the trajectory
  var_x:        The variance of the x coordinates of the trajectory
  var_y:        The variance of the y coordinates of the trajectory
  length:       The length of the trajectory
  scale:        The trajectory is computed on which scale
  x_pos:        The normalized x position w.r.t. the video (0~0.999), for spatio-temporal pyramid
  y_pos:        The normalized y position w.r.t. the video (0~0.999), for spatio-temporal pyramid
  t_pos:        The normalized t position w.r.t. the video (0~0.999), for spatio-temporal pyramid
  
  下面的5个部分是一个接一个连起来的：
  Trajectory:    2x[trajectory length] (default 30 dimension)
  HOG:           8x[spatial cells]x[spatial cells]x[temporal cells] (default 96 dimension)
  HOF:           9x[spatial cells]x[spatial cells]x[temporal cells] (default 108 dimension)
  MBHx:          8x[spatial cells]x[spatial cells]x[temporal cells] (default 96 dimension)
  MBHy:          8x[spatial cells]x[spatial cells]x[temporal cells] (default 96 dimension)
每隔设定的帧长度会从零开始再计算。
*/
#include "feature.h"

DTFeature::DTFeature()	{
	frameNum = -1;
// 轨迹特征参数
	mean_x = 0.0;
	mean_y = 0.0;
	var_x = 0.0;
	var_y = 0.0;
	length = 0.0;
	scale = 0.0;
	x_pos = 0.0;
	y_pos = 0.0;
	t_pos = 0.0;
//  Trajectory HOG HOF MBHx MBHy
	traj = NULL;
	hog = NULL;
	hof = NULL;
	mbhx = NULL;
	mbhy = NULL;
}

// 读取特征
DTFeature::DTFeature(string featureLine)	{
	stringstream ss;
	ss<<featureLine;//m每一行
	ss>>frameNum>>mean_x>>mean_y>>var_x>>var_y>>length>>scale>>x_pos>>y_pos>>t_pos;
	traj = new double[TRAJ_DIM];
	for (int i = 0; i < TRAJ_DIM; i++)
		ss>>traj[i];//轨迹
	hog = new double[HOG_DIM];
	for (int i = 0; i < HOG_DIM; i++)
		ss>>hog[i];
	hof = new double[HOF_DIM];
	for (int i = 0; i < HOF_DIM; i++)
		ss>>hof[i];
	mbhx = new double[MBHX_DIM];
	for (int i = 0; i < MBHX_DIM; i++)
		ss>>mbhx[i];
	mbhy = new double[MBHY_DIM];
	for (int i = 0; i < MBHY_DIM; i++)
		ss>>mbhy[i];
}

DTFeature::DTFeature(const DTFeature &f)	{
	frameNum = f.frameNum;
	mean_x = f.mean_x;
	mean_y = f.mean_y;
	var_x = f.var_x;
	var_y = f.var_y;
	length = f.length;
	scale = f.scale;
	x_pos = f.x_pos;
    y_pos = f.y_pos;
    t_pos = f.t_pos;

    traj = NULL;
    if (f.traj)   {
        traj = new double[TRAJ_DIM];
        for (int i = 0; i < TRAJ_DIM; i++)
            traj[i] = f.traj[i];
    }
    hog = NULL;
    if (f.hog)  {
        hog = new double[HOG_DIM];
        for (int i = 0; i < HOG_DIM; i++)
            hog[i] = f.hog[i];
    }
    hof = NULL;
    if (f.hof)  {
        hof = new double[HOF_DIM];
        for (int i = 0; i < HOF_DIM; i++)
            hof[i] = f.hof[i];
    }
    mbhx = NULL;
    if (f.mbhx) {
        mbhx = new double[MBHX_DIM];
        for (int i = 0; i < MBHX_DIM; i++)
            mbhx[i] = f.mbhx[i];
    }
    mbhy = NULL;
    if (f.mbhy) {
        mbhy = new double[MBHY_DIM];
        for (int i = 0; i < MBHY_DIM; i++)
            mbhy[i] = f.mbhy[i];
    }
}
//析构函数
DTFeature::~DTFeature()	{
    if (traj)
        delete [] traj;
    if (hog)
        delete [] hog;
    if (hof)
        delete [] hof;
    if (mbhx)
        delete [] mbhx;
    if (mbhy)
        delete [] mbhy;
}
// 等号赋值运算符重载
DTFeature &DTFeature::operator=(const DTFeature &f)	{
    if (this == &f)
        return *this;

	frameNum = f.frameNum;
	mean_x = f.mean_x;
	mean_y = f.mean_y;
	var_x = f.var_x;
	var_y = f.var_y;
	length = f.length;
	scale = f.scale;
	x_pos = f.x_pos;
    y_pos = f.y_pos;
    t_pos = f.t_pos;

    if (traj)
        delete []traj;
    traj = NULL;
    if (f.traj)   {
        traj = new double[TRAJ_DIM];
        for (int i = 0; i < TRAJ_DIM; i++)
            traj[i] = f.traj[i];
    }
    if (hog)
        delete [] hog;
    hog = NULL;
    if (f.hog)  {
        hog = new double[HOG_DIM];
        for (int i = 0; i < HOG_DIM; i++)
            hog[i] = f.hog[i];
    }
    if (hof)
        delete [] hof;
    hof = NULL;
    if (f.hof)  {
        hof = new double[HOF_DIM];
        for (int i = 0; i < HOF_DIM; i++)
            hof[i] = f.hof[i];
    }
    if (mbhx)
        delete [] mbhx;
    mbhx = NULL;
    if (f.mbhx) {
        mbhx = new double[MBHX_DIM];
        for (int i = 0; i < MBHX_DIM; i++)
            mbhx[i] = f.mbhx[i];
    }
    if (mbhy)
        delete [] mbhy;
    mbhy = NULL;
    if (f.mbhy) {
        mbhy = new double[MBHY_DIM];
        for (int i = 0; i < MBHY_DIM; i++)
            mbhy[i] = f.mbhy[i];
    }

    return *this;
}
