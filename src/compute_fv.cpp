/*
GMM高斯混合编码
计算 特征编码之后的特征
*/
#include "fisher.h"
#include "feature.h"

/*
 * A sample use of fisher vector coding with DT.
 * Require 5 PCA projection matrices and 5 GMM codebooks 
 * for each component of DT: TRAJ, HOG, HOF, MBHX, MBHY
 * Read input from stdin
 */

int main(int argc, char **argv) {
    if (argc < 4)   {
        cout<<"Usage: "<<argv[0]<<" pcaList codeBookList outputBase"<<endl;
        return 0;
    }
    // Important: GMM uses OpenMP to speed up
    // This will cause problem on cluster where all
    // cores from a node run this binary
    vl_set_num_threads(1);
    string pcaList(argv[1]);// pca 压缩系数
    string codeBookList(argv[2]);// 训练集训练好的码本 
    string outputBase(argv[3]);// 输出编码的特征的文件夹
    string types[5] = {"traj", "hog", "hof", "mbhx", "mbhy"};
    vector<FisherVector*> fvs(5, NULL);//指针数组

    ifstream fin1, fin2;
    fin1.open(pcaList.c_str());// pca c_str 转换成字符串打开
    if (!fin1.is_open())    {
        cout<<"Cannot open "<<pcaList<<endl;
        return 0;
    }
    fin2.open(codeBookList.c_str());// gmm 码本
    if (!fin2.is_open())    {
        cout<<"Cannot open "<<codeBookList<<endl;
        return 0;
    }
    string pcaFile, codeBookFile;
    for (int i = 0; i < fvs.size(); i++)    {
        getline(fin1, pcaFile);//  pca
        getline(fin2, codeBookFile);// gmm 码本
        //string d_path("../data");
        //pcaFile = d_path + pcaFile;
        //codeBookFile = d_path + codeBookFile;
        fvs[i] = new FisherVector(pcaFile, codeBookFile);
        // 
        fvs[i]->initFV(1);  // 1 layer of spatial pyramids
                            // 金字塔层数？？
    }
    fin1.close();
    fin2.close();

    string line;
    while (getline(cin, line))  {//idt子程序会输出 每一帧的特征
        DTFeature feat(line);
        // TODO: Store feature of DT with vector<double>
        // 保存idt特征 写到txt文件内
        vector<double> traj(feat.traj, feat.traj+TRAJ_DIM);//轨迹
        vector<double> hog(feat.hog, feat.hog+HOG_DIM);// 梯度直方图
        vector<double> hof(feat.hof, feat.hof+HOF_DIM);// 光流图
        vector<double> mbhx(feat.mbhx, feat.mbhx+MBHX_DIM);// 水平光流梯度直方图
        vector<double> mbhy(feat.mbhy, feat.mbhy+MBHY_DIM);// 垂直光流梯度直方图
        // 特征的几个部分
        fvs[0]->addPoint(traj, feat.x_pos, feat.y_pos);
        fvs[1]->addPoint(hog, feat.x_pos, feat.y_pos);
        fvs[2]->addPoint(hof, feat.x_pos, feat.y_pos);
        fvs[3]->addPoint(mbhx, feat.x_pos, feat.y_pos);
        fvs[4]->addPoint(mbhy, feat.x_pos, feat.y_pos);
    }
    cout<<"Points load complete."<<endl; 


    ofstream foutgfzy;//输出文件流 后面四种
    string outNamegfzy = outputBase + "." + "gfzy" + ".fv.txt";//分别保存
    foutgfzy.open(outNamegfzy.c_str());
    for (int i = 0; i < fvs.size(); i++)    {// 所有特征 
        ofstream fout;//输出文件流
        // TRAJ, HOG, HOF, MBHX, MBHY 分开编码
        string outName = outputBase + "." + types[i] + ".fv.txt";//分别保存
        fout.open(outName.c_str());
        // HOG, HOF, MBHX, MBHY 放在一起    
        // 获取编码向量
        vector<double> fv = fvs[i]->getFV();
        fout<<fv[0];
        foutgfzy<<fv[0];
        for (int j = 1; j < fv.size(); j++)
        {
            fout<<" "<<fv[j];//写入到文件保存
            foutgfzy<<" "<<fv[j];//写入到文件保存            
        }
        //foutgfzy << 
        fout<<endl;
        fout.close();
        fvs[i]->clearFV();
    }
    foutgfzy<<endl;
    foutgfzy.close();

    for (int i = 0; i < fvs.size(); i++)
        delete fvs[i];//释放内存
    return 0;
}
