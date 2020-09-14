//
// Created by max on 22.03.19.
//

#include "images.hpp"
#include "algorithm"
/*
static void top_N(float *a, int n, int k, int *index)
{
    int i,j;
    for(j = 0; j < k; ++j) index[j] = -1;
    for(i = 0; i < n; ++i){
        int curr = i;
        for(j = 0; j < k; ++j){
            if((index[j] < 0) || a[curr] > a[index[j]]){
                int swap = curr;
                curr = index[j];
                index[j] = swap;
            }
        }
    }
}

*/

void checkme(const std::vector<std::string> &labels, const std::vector<std::vector<boost::filesystem::path> > &imgs,
             boost::filesystem::path cfgfile, boost::filesystem::path weightfile, int gpu)  {
    std::cout << "Loading network from "<< cfgfile << std::endl;

    char *base = basecfg((char*)cfgfile.c_str());
    printf("%s\n", base);
    network *net;


#ifdef GPU
    cuda_set_device(gpu);
#endif
    net = load_network((char*)cfgfile.c_str(), (char*)weightfile.c_str(), 0);
    set_batch_network(net, 1);
    networkSizeImageInput netp(net);

    int ok=0;
    int err=0;
    for(int dset=0; dset<imgs.size(); dset++) {

        int okX=0;
        int errX=0;
         BOOST_FOREACH(boost::filesystem::path imgname, imgs[dset]) {
                        //  std::cout << "\n *** --- --- ---  " << imgname << "  --- --- --- ***" << std::endl;
                        imageCropLoader image(imgname, netp);
                        float *pred = network_predict_image(net, image.getImage());
//        float *p = network_predict(.data);
                        //std::cout << "\n *** --- --- ---  " << imgname << std::endl;


                        //top_N(pred, labels.size(), topk, indexes);
                        float maxval=-1.0;
                        int maxid=-1;
                        for (int i = 0; i < labels.size(); i++) {
                            //if (pred[i] != 0.) std::cout << labels[i] << "/" << i << ": " << pred[i] << "\n";
                            if(maxval<pred[i]) {
                                maxval=pred[i];
                                maxid=i;
                            }

//                            pred[i] = 0.;
                        }
                        if(maxid==dset) {ok++; okX++; } else {err++; errX++; }
                    }
        std::cout << labels[dset] << " !!! " << okX << "/" << errX << " % "<< float(okX)/float(errX+okX) <<std::endl;
    }
    std::cout << ok << "/" << err << " % "<< float(ok)/float(err+ok) <<std::endl;

}

int main(int argc, char *argv[]) {
    std::vector<int> gpus(1);

    auto e = boost::this_process::environment();
    std::list<boost::filesystem::path> path;
    path.push_back( boost::filesystem::path(e["HOME"].to_string()) );


    std::istringstream is(e["DATA"].to_string());
    std::string s;

    while (std::getline(is, s, ':')) {
        boost::filesystem::path newchunk(s);
        if( !boost::filesystem::exists(newchunk) ) {
            std::cerr << newchunk << " path not exists" << std::endl;
            continue;
        }

        if(!boost::filesystem::is_directory(newchunk)) {
            std::cerr << newchunk << " path not directory" << std::endl;
            continue;
        }

        //    std::cerr << newchunk << " appended" << std::endl;

        path.push_back(newchunk);
    }


    //auto root=home / "break";

    //root=boost::filesystem::current_path();

    //environment e = this_process::environment();
    //std::list<boost::filesystem::path> file;


    std::cout << "Path:" << std::endl;
    for(auto it = path.begin(); it != path.end(); ++it) {
        std::cout << *it << std::endl;
    }

    boost::filesystem::path trainDir, backupDir, weightFile, networkCfg;

    if( !(  checkDirPath(trainDir,    "data/train",        path)  &&
            checkDirPath(backupDir,   "data/backup",       path)  &&
            checkFilePath(networkCfg, "data/network.cfg",  path)  ) /**/ ) {

        std::cout << "Data not found" << std::endl;
        return 1;

    }

    std::vector<std::string> names;

    boost::filesystem::directory_iterator it{trainDir};
    // std::cout << *it++ << '\n';
    while (it != boost::filesystem::directory_iterator{}) {
        auto cwd=(*it++).path();
        auto name=cwd.filename().string();
        names.push_back(name);

    }
    std::sort(names.begin(), names.end(), std::greater<std::string>());

    std::cout << "-------------- 1 ----------------" << std::endl;

    std::vector<std::vector<boost::filesystem::path>> imgs(names.size());
    int ns=0;
    BOOST_FOREACH( std::string namel, names )
                {
                    boost::filesystem::path imgDir=trainDir/boost::filesystem::path(namel);
                    std::cout << imgDir << std::endl;

                    boost::filesystem::directory_iterator it{imgDir};
                    // std::cout << *it++ << '\n';
                    while (it != boost::filesystem::directory_iterator{}) {
                        auto cwd=(*it++).path();
                        //auto name=cwd.filename().string();
                        imgs[ns].push_back(cwd);
                    }

                    std::cout << " == " << namel << " -> " << imgs[ns].size() << std::endl;
                    ns++;
                }

    std::cout << "-------------- 2 ----------------" << std::endl;

    int clear=!checkFilePath(weightFile, "data/weights.data", path);
    if(clear) {
        std::cerr << "FATAL: Missing trained network (weights.data)" << std::endl;
        return 1;
    }

    checkme(names, imgs, networkCfg, weightFile, gpus[0]);

//    trainme(names, imgs, networkCfg, weightFile, backupDir, &gpus[0], gpus.size(), clear);
//    checkDirPath(file, "data.test", path);
    //std::cout << "Hello world\n" << "from " << root << std::endl;
    return 0;

//  trainme("tennis/data.list", "tennis/network_topology.cfg", "tennis/networkweights.cfg", "tennis/backup", &gpus[0], gpus.size() ,0) ;
}
