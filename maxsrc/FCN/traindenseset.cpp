//
// Created by max on 19.09.2020.
//
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <network.h>
#include <parser.h>
#include <cmath>
#include <ctime>
#include "darknet.h"
#include "matrix.h"
#include "customdata.hpp"
#include "batchset.h"
//#include "jsonset.h"

extern "C" {
#include "utils.h"
}
#include "option_list.h"
#include "jsonset.h"



struct networkInput {
    int size;
    int batch;
    networkInput(int s): size(s), batch(1) { }

    virtual int getWidth() const { return  size; }
    virtual const int getBatch() const { return  batch; }
};
/*
class dirRandomLabelDataBuilder: public dataBuilder {
    const std::vector<std::vector<boost::filesystem::path>> imgs;
    const std::vector<std::string> labels;
    const networkInput &netParam;
    int batch;
public:
    dirRandomLabelDataBuilder(const  std::vector<std::vector<boost::filesystem::path>> &im,
                              const  std::vector<std::string> &l, const networkInput &p, int batchSize=32):
            imgs(im), labels(l),  netParam(p), batch(batchSize) { }


    void setBatch(int b) {
        batch=b;
    }

    data buildData() override {
        data d={0, 0};

        int nLabels=imgs.size();


        d.shallow = 0;
        d.w=netParam.getWidth();
        d.h=netParam.getBatch();
        d.X=verticalVector(batch);
        d.y=make_matrix(batch, nLabels);

        for(int i=0; i<batch; i++) {
// choose random image
            int iLabel=rand()%imgs.size();
            int iImage=rand()%imgs[iLabel].size();
            auto path=imgs[iLabel][iImage];

// load choosen image
            rollingdata datain();
            //imageCropLoader loader(path, netParam);
            //json
            d.X.vals[i]=datain().buf;//loader.getImage().data;
// set label's Neuron to 1.0 (and rest is 0) - target output
            auto targetVector=d.y.vals[i];
            memset(targetVector, 0, sizeof(targetVector[0])*nLabels);
            //std::cerr << iLabel << " ";
            targetVector[iLabel]=1.0;
        }



        return d;
/ *
     // label paths (char **paths, int n, char **labels, int k, tree *hierarchy)

        if(m) paths = me_get_random_paths(paths, n, m);
        data d = {0};
        d.shallow = 0;
        d.w=size;
        d.h=size;
// !!
        d.X = me_load_image_augment_paths(paths, n, min, max, size, angle, aspect, hue, saturation, exposure, center);
        d.y = me_load_labels_paths(paths, n, labels, k, hierarchy);
        data d={0, 0};

        return d;
/   * /
    }

};
*/

void trainme(const boost::filesystem::path in, const boost::filesystem::path out,
             boost::filesystem::path cfgfile, boost::filesystem::path weightfile,
             boost::filesystem::path backup_directory,
             std::vector<int> &gpus, int timescale, int classes=2)  {


    float avg_loss = -1;

    std::cout << "Loading network from "<< cfgfile << std::endl;

    char *base = basecfg( (char*)cfgfile.c_str() );

    printf("Network: %s\n", base);
    printf("Gpu count:  %lu\n", gpus.size());
    network **nets = (network **)calloc(gpus.size(), sizeof(network*));

    srand(time(0) );

    //
    int seed = rand();
    bool clear=weightfile.empty();

    for(uint i = 0; i < gpus.size(); ++i){
        srand(seed);
#ifdef GPU
        std::cout << "Set gpu " << gpus[i] << std::endl;
        cuda_set_device(gpus[i]);
#endif
        nets[i] = load_network((char*)cfgfile.c_str(), (char*)(clear?"":weightfile.c_str()), clear);
        nets[i]->learning_rate *= gpus.size();
    }
    srand(time(0));
    network *net = nets[0];

    int imgBlock = net->batch * net->subdivisions * gpus.size();
    std::cout << "Block size: "<< imgBlock << std::endl;

    printf("Learning Rate: %g, Momentum: %g, Decay: %g\n", net->learning_rate, net->momentum, net->decay);
    list *options = read_data_cfg((char*)cfgfile.c_str());

    //int N = 0;
/*
    BOOST_FOREACH( const std::vector<boost::filesystem::path> &namel, imgs )
    {
        N+=namel.size();
    }
*/
    // = option_find_str(options, "backup", "/backup/");
    //int tag = option_find_int_quiet(options, (char*)"tag", 0);

    //char *label_list = option_find_str(options, "labels", "data/labels.list");
    //char *train_list = option_find_str(options, "train", "data/train.list");
    //char *tree = option_find_str(options, "tree", 0);

    //
    //
    // if (tree) net->hierarchy = read_tree(tree);

    classes = option_find_int(options, "classes", classes);
    std::cerr << "classes: " << classes << std::endl;
    //char **labels = 0;
    //labels = get_labels(label_list);
    //list *plist = get_paths(train_list);
    //char **paths = (char **)list_to_array(plist);
    //printf("%d\n", plist->size);


    /*load_args args = {0};
    args.w = net->w;
    args.h = net->h;
    args.threads = 32;
    args.hierarchy = net->hierarchy;

    args.min = net->min_ratio*net->w;
    args.max = net->max_ratio*net->w;
    printf("%d %d\n", args.min, args.max);
    args.angle = net->angle;
    args.aspect = net->aspect;
    args.exposure = net->exposure;
    args.saturation = net->saturation;
    args.hue = net->hue;
    args.size = net->w;

    args.paths = paths;
    args.classes = classes;
    args.n = imgs;
    args.m = N;
    args.labels = labels;

    args.type = CLASSIFICATION_DATA;
     // load_data_augment(a.paths, a.n, a.m, a.labels, a.classes, a.hierarchy, a.min, a.max, a.size, a.angle, a.aspect, a.hue, a.saturation, a.exposure, a.center);
*/
    std::ifstream indata(in.string());
    std::ifstream outdata(out.string());
    //return;

    //data buffer;
//    std::cout << vin.size() << ":" << vout.size() << std::endl;
    //std::vector<std::vector<boost::filesystem::path>> &im,
    //std::vector<std::string> &l, networkImageInput &p,
    //int epoch = (*net->seen)/N;

    //networkSizeImageInput netp(net);

//    dirRandomLabelDataBuilder dataBuilder(imgs, labels, netp, 2048 /*?some?*/);

    std::cout << "Building data batch" << std::endl;
    //train=dataBuilder.buildData();
/*


    std::cout << "Data batch loaded" << std::endl;


      pthread_t load_thread;
      args.d = &buffer;
      load_thread = me_load_data(args);

      int count = 0;
      while(get_current_batch(net) < net->max_batches || net->max_batches == 0){
          if(net->random && count++%40 == 0){
              printf("Resizing\n");
              int dim = (rand() % 11 + 4) * 32;
              //if (get_current_batch(net)+200 > net->max_batches) dim = 608;
              //int dim = (rand() % 4 + 16) * 32;
              printf("%d\n", dim);
              args.w = dim;
              args.h = dim;
              args.size = dim;
              args.min = net->min_ratio*dim;
              args.max = net->max_ratio*dim;
              printf("%d %d\n", args.min, args.max);

              pthread_join(load_thread, 0);
              train = buffer;
              free_data(train);
              load_thread = load_data(args);

              for(i = 0; i < ngpus; ++i){
                  resize_network(nets[i], dim, dim);
              }
              net = nets[0];
          }

          pthread_join(load_thread, 0);
    */

    //train = buffer;



    //      load_thread = load_data(args);

   // double ftime;

   // ftime = what_time_is_it_now();
    //printf("Loaded: %lf seconds\n", what_time_is_it_now()-time);
   double ftime = what_time_is_it_now();
    char trainid[256];
    sprintf(trainid, "%ld", (long)time(0));

    float loss = 0;
    double avgloss = 100;
    double minavgloss = MAXFLOAT;
    int    invalidloss=0;
    const int scale=net->max_batches;

    int N=1;

    int inputs=net->inputs;
    int outputs=net->outputs;

    assert(gpus.size() == 1);  // to simplify for now
    std::cerr << "Train network single GPU (repeat max "<< scale <<" times max "<<timescale<<"s)" << std::endl;

    std::vector<std::vector<float>> allInDense;

    readjsonarrays(indata, allInDense);
    int samples=allInDense.size();
    if(samples==0) {
        std::cerr << "No input data" << std::endl;
        return;
    } else {
        std::cout << samples << "  inputs loaded" << std::endl;
    }

    int samplerow=allInDense[0].size();
    if(samplerow==0 || (inputs % samplerow)!=0) {
        std::cerr << "Invalid row data " << samplerow << std::endl;
        return;
        }
    else {
        std::cout << samplerow << " inputs in each row sample" << std::endl;
    }
    auto sparseIn=mergeVectorRev(allInDense, inputs/samplerow);

    std::vector<int>  allOutDense;
    readjsonarray(outdata, allOutDense);

    auto sparseOut=makeSparseVectorRev(allOutDense, allInDense.size(), inputs/allInDense[0].size());

    const int K=1;  // number of batches in one go
//    int learnRows=net->batch*K;

    batchset traindatabatch(net, K);

    //std::ofstream outout("expected_out.log");
    //std::ofstream outdump("all_out.data");
    //sparseOut.Print(outdump);
    double saveat=ftime;
    if (sparseIn.rowsCount != sparseOut.rowsCount) {
        std::cerr << "sparseIn.rowsCount != sparseOut.rowsCount" << std::endl;
    }
    //volatile int sparse=0;
    for(int i=0; i<scale  && (what_time_is_it_now()-ftime)<timescale; i++) {

        if(!traindatabatch.datasetrows( [&allOutDense,  samples, &sparseIn, &sparseOut, inputs, outputs](float *&pin, float *&pout) {
                //sparse++;
                if(rand_int(0, 20)>3) {
                    int vIn = rand_int(2, -2);
                    int vIn2 = allOutDense[rand_int(0, allOutDense.size() - 1)];
                    pin = sparseIn.RowOrig(vIn + vIn2);
                    pout = sparseOut.RowOrig(vIn + vIn2);
                }
                else {
                   int vIn=rand_int(0, sparseIn.rowsCount-1);
                   pin=sparseIn.RowOrig(vIn);
                   pout=sparseOut.RowOrig(vIn);
                }

                //outout << vIn2<< ":" << vIn << ":[" << pout[0] <<", "<< pout[1] << "]"<<  std::endl;
                return true;
                } // end of lambda expression)
            )) break;

        loss = train_network(*net, traindatabatch);
        if( std::isfinite(loss) ) {
            avgloss=(avgloss*999+loss)/1000;
            if(avgloss<minavgloss) minavgloss=avgloss;
        } else {
            invalidloss++;
        }

       if (i%1000==0) {
           std::cout << "Min-Avg: "<< minavgloss  << "  Avg: "<< avgloss << "  Invalid:" << invalidloss <<  "  Recent:" << loss << " at "  << i << " by " <<what_time_is_it_now()-ftime  << "s" << std::endl;
           invalidloss=0;
           double tnow=what_time_is_it_now();
           if(tnow-saveat>10.) {
               char buf[128];
               sprintf(buf, "%s.%ld", trainid, long(tnow-ftime));
               auto newpath = backup_directory / buf;
               std::cout << "Saving weights: " << newpath << std::endl;
               save_weights(*net, (char *) (newpath).c_str());
               saveat=tnow;
               }

           }
        N++;

       }

//    delete []valsIns;
//    delete []valsOuts;

    if(avg_loss == -1) avg_loss = loss;
    avg_loss = avg_loss*.99 + loss*.01;
    printf("%d, %.3f: %f, %f avg, %f rate, %lf seconds, %ld images\n", get_current_batch(*net), (float)(*net->seen)/N, loss, avg_loss, get_current_rate(*net), what_time_is_it_now()-ftime, *net->seen);
    //free_data(train);
/*
    if(*net->seen/N > epoch) {
        epoch = *net->seen/N;
        char buff[256];
        sprintf(buff, "%s/%s_%d.weights",backup_directory.c_str(),base, epoch);
        save_weights(*net, buff);
    }*/
    //if(get_current_batch(*net)%1000 == 0){
    //    char buff[256];
    //    sprintf(buff, "%s/%s.backup",backup_directory.c_str(),base);
    //    save_weights(*net, buff);
    //}


    //char buff[256];

    //sprintf(buff, "%s/%s.weights", backup_directory.c_str(), base);
//    pthread_join(load_thread, 0);

    {
        char buf[128];
        sprintf(buf, "%s.final", trainid);
        auto newpath = backup_directory / buf;
        std::cout << "Saving weights: " << newpath << std::endl;
        save_weights(*net, (char *) (newpath).c_str());
    }
    free_network(*net);
    //free_list(plist);
   // free(base);
}

static bool IsFile(boost::filesystem::path &path, std::string spath) {

    if(!boost::filesystem::exists(spath) || !boost::filesystem::is_regular_file(spath)) {
        std::cerr << spath << " not a file" << std::endl;
        return false;
    }
    path=boost::filesystem::path(spath);
    return true;
}

static bool IsDir(boost::filesystem::path &path, std::string spath) {

    if(!boost::filesystem::exists(spath) || !boost::filesystem::is_directory(spath)) {
        std::cerr << spath << " not a directory" << std::endl;
        return false;
    }
    path=boost::filesystem::path(spath);
    return true;
}


int main(int narg, char *sarg[]) {
    //
    std::vector<int> gpus(1);
    //
    std::cout << "" << std::endl;
    //
    namespace po = boost::program_options;

    std::string cfg_path, backup_path, weight_path, in_data_path,  out_data_path;
    float thresh=40.0;
    int timelimit=600;
    long max_loop=100000;
    po::options_description desc("options");

//    std::string task_type;
    //std::string datamapconfig("/home/max/Videos/map.json");
    // long dbdatasize=2048;

    try {

        desc.add_options()
                ("help,h", "Show help")
                ("backup,b", po::value<std::string>(&backup_path), "Backup dir path")
                ("netcfg,c", po::value<std::string>(&cfg_path), "Config file path")
                ("weight,w", po::value<std::string>(&weight_path), "Weight file(s) path")
                ("input,i", po::value<std::string>(&in_data_path), "Input data")
                ("threshold,t", po::value<float>(&thresh),  "Threshold to stop testing 0-100%")
                ("timelimit,s", po::value<int>(&timelimit),  "Time limit (seconds)")
                ("expected,x", po::value<std::string>(&out_data_path), "Expected data file")
                ("maxloop,m", po::value<long>(&max_loop), "Data output file");

        po::positional_options_description pos_desc;
        pos_desc.add("netcfg", 1);


        pos_desc.add("input", 1);
        pos_desc.add("expected", 1);

        pos_desc.add("backup", 1);
        pos_desc.add("threshold", 1);
        pos_desc.add("weight", 1);

        po::parsed_options parsed = po::command_line_parser(narg, sarg).options(desc).positional(pos_desc).run();
        po::variables_map vm;
        po::store(parsed, vm);
        po::notify(vm);

        if (cfg_path.empty()      || // weight_path.empty()    ||
            in_data_path.empty() ||  out_data_path.empty() ||
            backup_path.empty() /*|| class_map_path.empty()*/ || vm.count("help")) {
            std::cout << "Cmd [options] cfg in exp backup [thresh] [weight]\n" << desc << std::endl;
            return 1;
        }

        //classes.readSet( class_map_path );

    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
        return 2;
    }


    boost::filesystem::path  backupDir, weightFile, networkCfg, inputFile, outputFile;

    if(  !IsDir(backupDir,  backup_path)  |
         (!weightFile.empty() && !IsFile( weightFile, weight_path)) |
         !IsFile( networkCfg, cfg_path) |
         !IsFile( inputFile, in_data_path) |
         !IsFile( outputFile, out_data_path)
        ) {
            return 1;

        }


//    std::vector<std::vector<boost::filesystem::path>> imgs(names.size());

/*
void trainme(boost::filesystem::path cfgfile, boost::filesystem::path weightfile,
               boost::filesystem::path backup_directory,
                         int *gpus, int ngpus, int clear, int classes=2)

 */

    trainme(inputFile, outputFile,  networkCfg, weightFile, backupDir, gpus, timelimit);
//    checkDirPath(file, "data.test", path);
    //std::cout << "Hello world\n" << "from " << root << std::endl;
    return 0;

//  trainme("tennis/data.list", "tennis/network_topology.cfg", "tennis/networkweights.cfg", "tennis/backup", &gpus[0], gpus.size() ,0) ;
}

