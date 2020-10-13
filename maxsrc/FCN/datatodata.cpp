//
// Copied by max on 04.10.2020.
// based on scantodata
//
//scanopt

#include "network.h"
#include "detection_layer.h"
#include "region_layer.h"
#include "cost_layer.h"
#include "utils.h"
#include "parser.h"
#include "box.h"
#include "image.h"
#include "demo.h"
#include "darknet.h"
#include <sys/time.h>
#include <iostream>
#include "http_stream.h"
#include "jsonset.h"

#include "../kwset.h"
//
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include "rapidjson/stringbuffer.h"
#include <rapidjson/writer.h>
#include <rapidjson/allocators.h>
//#include <chrono>
//#include <map>
//#include <set>
#include <boost/program_options.hpp>
#include <fstream>
//
//static const char **demo_names;
//static image **demo_alphabet;
//static int demo_classes;

//static int nboxes = 0;
//static detection *dets = NULL;

static network net;
//static image in_s ;
//static image det_s;

//static float demo_thresh = 0; // passed to thresholds
//static int demo_ext_output = 0;
//static long long int frame_id = 0;
//static int demo_json_port = -1;

//#define NFRAMES 3

//static float* predictions[NFRAMES];
//static int demo_index = 0;

//static float *avg;

void scantodata(const char *cfgfile,
                const char *weightfile,
                const char *in_filename,
                const char *out_filename);

int main(int narg, char **sarg) {
    //
    std::cout << "To data scan" << std::endl;
    //
    namespace po = boost::program_options;
/*
    scantodata( const char *filename, char **names, int classes,
    int frame_skip, char *prefix,  int dont_show, int ext_output,
    ); */
    std::string cfg_path,      weight_path,
                in_data_path,  out_data_path;
    int out_width;

    //classSet classes;
    po::options_description desc("options");

//    std::string task_type;
    //std::string datamapconfig("/home/max/Videos/map.json");
   // long dbdatasize=2048;

    try {

/*
 *  ... -i /home/max/Videos/shade/tennis3.shades.mkv -c yolov3-tiny.cfg -w yolov3-tiny.weights -m obj.names -o labels.mp4 -j labels.mp4.json
 */
        desc.add_options()
                ("help,h", "Show help")
                ("cfg,c", po::value<std::string>(&cfg_path), "Config file path")
                ("weight,w", po::value<std::string>(&weight_path), "Weight file path")
                //("map,m", po::value<std::string>(&class_map_path), "Class map")
                ("outwidth,u", po::value<int>(&out_width),  "Threshold probability in percents 0-100")
                ("indata,i", po::value<std::string>(&in_data_path), "Video input file")
                ("outdata,j", po::value<std::string>(&out_data_path), "Data output file");

        po::positional_options_description pos_desc;
        pos_desc.add("cfg", 1);
        pos_desc.add("weight", 1);
        pos_desc.add("indata", 1);
        pos_desc.add("outdata", 1);

        po::parsed_options parsed = po::command_line_parser(narg, sarg).options(desc).positional(pos_desc).run();
        po::variables_map vm;
        po::store(parsed, vm);
        po::notify(vm);

        if (cfg_path.empty()      || weight_path.empty()    ||
            in_data_path.empty() || //out_video_path.empty() ||
            out_data_path.empty() || vm.count("help")) {
            std::cout << "Cmd [options]\n" << desc << std::endl;
            return 1;
        }

    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
        return 2;
        }

    scantodata(cfg_path.c_str(),weight_path.c_str(),
              in_data_path.c_str(), out_data_path.c_str() );

/*
    scantodata( cfg_path.c_str(), weight_path.c_str(), thresh/100.,
                     in_data_path.c_str() , classes.names , classes.classes,
                     out_data_path.c_str(), out_video_path.c_str());
*/
    //classes.freeBuf();

    return 0;
}
/*
void demo(char *cfgfile, char *weightfile, float thresh, float hier_thresh, int cam_index, const char *filename, char **names, int classes,
          int frame_skip, char *prefix, char *out_filename, int mjpeg_port, int json_port, int dont_show, int ext_output, int letter_box_in, int time_limit_sec, char *http_post_host,
          int benchmark, int benchmark_layers)
{
    letter_box = letter_box_in;
    in_img = det_img = show_img = NULL;
    //skip = frame_skip;
    image **alphabet = load_alphabet();networkSizeImageInput netp(net);
    int delay = frame_skip;
    demo_names = names;
    demo_alphabet = alphabet;
    demo_classes = classes;
    demo_thresh = thresh;
    demo_ext_output = ext_output;
    demo_json_port = json_port;
    printf("Demo\n");
    net = parse_network_cfg_custom(cfgfile, 1, 1);    // set batch=1
    if(weightfile){
        load_weights(&net, weightfile);
    }
    net.benchmark_layers = benchmark_layers;
    fuse_conv_batchnorm(net);
    calculate_binary_weights(net);
    srand(2222222);

    if(filename){
        printf("video file: %s\n", filename);
        cap = get_capture_video_stream(filename);
    }else{
        printf("Webcam index: %d\n", cam_index);
        cap = get_capture_webcam(cam_index);
    }

    if (!cap) {
        error("Couldn't connect to webcam.\n");
    }

    layer l = net.layers[net.n-1];
    int j;

    avg = (float *) calloc(l.outputs, sizeof(float));
    for(j = 0; j < NFRAMES; ++j) predictions[j] = (float *) calloc(l.outputs, sizeof(float));

    if (l.classes != demo_classes) {
        printf("\n Parameters don't match: in cfg-file classes=%d, in data-file classes=%d \n", l.classes, demo_classes);
        getchar();
        exit(0);
    }

    flag_exit = 0;

    custom_thread_t fetch_thread = NULL;
    custom_thread_t detect_thread = NULL;
    if (custom_create_thread(&fetch_thread, 0, fetch_in_thread, 0)) error("Thread creation failed");
    if (custom_create_thread(&detect_thread, 0, detect_in_thread, 0)) error("Thread creation failed");

    fetch_in_thread_sync(0); //fetch_in_thread(0);
    det_img = in_img;
    det_s = in_s;

    fetch_in_thread_sync(0); //fetch_in_thread(0);
    detect_in_thread_sync(0); //fetch_in_thread(0);
    det_img = in_img;
    det_s = in_s;

    for (j = 0; j < NFRAMES / 2; ++j) {
        free_detections(dets, nboxes);
        fetch_in_thread_sync(0); //fetch_in_thread(0);
        detect_in_thread_sync(0); //fetch_in_thread(0);
        det_img = in_img;
        det_s = in_s;
    }

    int count = 0;
    if(!prefix && !dont_show){
        int full_screen = 0;
        create_window_cv("Demo", full_screen, 1352, 1013);
    }


    write_cv* output_video_writer = NULL;
    if (out_filename && !flag_exit)
    {
        int src_fps = 25;
        src_fps = get_stream_fps_cpp_cv(cap);
        output_video_writer =
                create_video_writer(out_filename, 'H', '2', '6', '4', src_fps, get_width_mat(det_img), get_height_mat(det_img), 1);

        //'H', '2', '6', '4'
        //'D', 'I', 'V', 'X'
        //'M', 'J', 'P', 'G'
        //'M', 'P', '4', 'V'
        //'M', 'P', '4', '2'
        //'X', 'V', 'I', 'D'
        //'W', 'M', 'V', '2'
    }

    int send_http_post_once = 0;
    const double start_time_lim = get_time_point();
    double before = get_time_point();
    double start_time = get_time_point();
    float avg_fps = 0;
    int frame_counter = 0;

    while(1 //  && count<2400
        ){
        ++count;
        {
            const float nms = .45;    // 0.4F
            int local_nboxes = nboxes;
            detection *local_dets = dets;
            this_thread_yield();

            if (!benchmark) custom_atomic_store_int(&run_fetch_in_thread, 1); // if (custom_create_thread(&fetch_thread, 0, fetch_in_thread, 0)) error("Thread creation failed");
            custom_atomic_store_int(&run_detect_in_thread, 1); // if (custom_create_thread(&detect_thread, 0, detect_in_thread, 0)) error("Thread creation failed");

            //if (nms) do_nms_obj(local_dets, local_nboxes, l.classes, nms);    // bad results
            if (nms) {
                if (l.nms_kind == DEFAULT_NMS) do_nms_sort(local_dets, local_nboxes, l.classes, nms);
                else diounms_sort(local_dets, local_nboxes, l.classes, nms, l.nms_kind, l.beta_nms);
            }

            //printf("\033[2J");
            //printf("\033[1;1H");
            //printf("\nFPS:%.1f\n", fps);
            printf("Objects:\n\n");

            ++frame_id;
            if (demo_json_port > 0) {
                int timeout = 400000;
                send_json(local_dets, local_nboxes, l.classes, demo_names, frame_id, demo_json_port, timeout);
            }

            //char *http_post_server = "webhook.site/898bbd9b-0ddd-49cf-b81d-1f56be98d870";
            if (http_post_host && !send_http_post_once) {
                int timeout = 3;            // 3 seconds
                int http_post_port = 80;    // 443 https, 80 http
                if (send_http_post_request(http_post_host, http_post_port, filename,
                                           local_dets, nboxes, classes, names, frame_id, ext_output, timeout))
                {
                    if (time_limit_sec > 0) send_http_post_once = 1;
                }
            }

            if (!benchmark) draw_detections_cv_v3(show_img, local_dets, local_nboxes, demo_thresh, demo_names, demo_alphabet, demo_classes, demo_ext_output);
            free_detections(local_dets, local_nboxes);

            printf("\nFPS:%.1f \t AVG_FPS:%.1f\n", fps, avg_fps);

            if(!prefix){
                if (!dont_show) {
                    show_image_mat(show_img, "Demo");
                    int c = wait_key_cv(1);
                    if (c == 10) {
                        if (frame_skip == 0) frame_skip = 60;
                        else if (frame_skip == 4) frame_skip = 0;
                        else if (frame_skip == 60) frame_skip = 4;
                        else frame_skip = 0;
                    }
                    else if (c == 27 || c == 1048603) // ESC - exit (OpenCV 2.x / 3.x)
                    {
                        flag_exit = 1;
                    }
                }
            }else{
                char buff[256];
                sprintf(buff, "%s_%08d.jpg", prefix, count);
                if(show_img) save_cv_jpg(show_img, buff);
            }

            // if you run it with param -mjpeg_port 8090  then open URL in your web-browser: http://localhost:8090
            if (mjpeg_port > 0 && show_img) {
                int port = mjpeg_port;
                int timeout = 400000;
                int jpeg_quality = 40;    // 1 - 100
                send_mjpeg(show_img, port, timeout, jpeg_quality);
            }

            // save video file
            if (output_video_writer && show_img) {
                write_frame_cv(output_video_writer, show_img);
                printf("\n cvWriteFrame \n");
            }

            while (custom_atomic_load_int(&run_detect_in_thread)) {
                if(avg_fps > 180) this_thread_yield();
                else this_thread_sleep_for(thread_wait_ms);   // custom_join(detect_thread, 0);
            }
            if (!benchmark) {
                while (custom_atomic_load_int(&run_fetch_in_thread)) {
                    if (avg_fps > 180) this_thread_yield();
                    else this_thread_sleep_for(thread_wait_ms);   // custom_join(fetch_thread, 0);
                }
                free_image(det_s);
            }

            if (time_limit_sec > 0 && (get_time_point() - start_time_lim)/1000000 > time_limit_sec) {
                printf(" start_time_lim = %f, get_time_point() = %f, time spent = %f \n", start_time_lim, get_time_point(), get_time_point() - start_time_lim);
                break;
            }

            if (flag_exit == 1) break;

            if(delay == 0){
                if(!benchmark) release_mat(&show_img);
                show_img = det_img;
            }
            det_img = in_img;
            det_s = in_s;
        }
        --delay;
        if(delay < 0){
            delay = frame_skip;

            //double after = get_wall_time();
            //float curr = 1./(after - before);
            double after = get_time_point();    // more accurate time measurements
            float curr = 1000000. / (after - before);
            fps = fps*0.9 + curr*0.1;
            before = after;

            float spent_time = (get_time_point() - start_time) / 1000000;
            frame_counter++;
            if (spent_time >= 3.0f) {
                //printf(" spent_time = %f \n", spent_time);
                avg_fps = frame_counter / spent_time;
                frame_counter = 0;
                start_time = get_time_point();
            }
        }
    }
    printf("input video stream closed. \n");
    if (output_video_writer) {
        release_video_writer(&output_video_writer);
        printf("output_video_writer closed. \n");
    }

    this_thread_sleep_for(thread_wait_ms);

    custom_join(detect_thread, 0);
    custom_join(fetch_thread, 0);

    // free memory
    free_image(in_s);
    free_detections(dets, nboxes);

    free(avg);
    for (j = 0; j < NFRAMES; ++j) free(predictions[j]);
    demo_index = (NFRAMES + demo_index - 1) % NFRAMES;
    for (j = 0; j < NFRAMES; ++j) {
        release_mat(&cv_images[j]);
    }

    free_ptrs((void **)names, net.layers[net.n - 1].classes);

    int i;
    const int nsize = 8;
    for (j = 0; j < nsize; ++j) {
        for (i = 32; i < 127; ++i) {
            free_image(alphabet[j][i]);
        }
        free(alphabet[j]);
    }
    free(alphabet);
    free_network(net);
    //cudaProfilerStop();
}
*/

void scantodata(const char *cfgfile, const char *weightfile,
                const char *in_filename,
                const char *out_filename)  {


    //int benchmark,
//    letter_box = letter_box_in;

    //skip = frame_skip;
    //image **alphabet = load_alphabet();
    //demo_names = names;
//    demo_alphabet = alphabet;
//    demo_classes = classes;
//    demo_thresh = thresh;

//    demo_json_port = json_port;
    printf("Reading network %s\n",cfgfile);
    net = parse_network_cfg_custom((char*)cfgfile, 1, 1);    // set batch=1

    if(weightfile) {
        printf("Reading network weights %s ...\n",weightfile);
        load_weights(&net, (char*)weightfile);
        }
    else {
        printf("No network weights %s\n",cfgfile);
        exit(2);
        }

   // net.benchmark_layers = benchmark_layers;
    //fuse_conv_batchnorm(net);       // Optimisation only ??
    calculate_binary_weights(net);  // Optimisation only ??

    std::ifstream datain(in_filename);

    if(!datain) {
        printf("\n Failed opening %s \n", in_filename);
        //getchar();
        exit(0);
        }
    else {
        printf("\nReading %s \n", in_filename);
        }

    std::ofstream dataout(out_filename);

    if(!dataout) {
        printf("\n Failed opening %s \n", out_filename);
        //getchar();
        exit(0);
        }
    else {
        printf("\nWriting %s \n", out_filename);
        }

    srand(2222222);

/*
    if (!cap) {
        error("Couldn't connect to webcam.\n");
    }
*/
    layer l = net.layers[net.n-1];
    int j;

//    avg = (float *) calloc(l.outputs, sizeof(float));

    //for(j = 0; j < l.outputs; ++j) predictions[j] = (float *) calloc(l.outputs, sizeof(float));
/*
    if (l.classes != classes) {
        printf("\n Parameters don't match: in cfg-file classes=%d, in data-file classes=%d \n", l.classes, classes);
        //getchar();
        exit(0);
    }
*/

    //custom_thread_t fetch_thread = NULL;
    //custom_thread_t detect_thread = NULL;
    //if (custom_create_thread(&fetch_thread, 0, fetch_in_thread, 0)) error("Thread creation failed");
    //if (custom_create_thread(&detect_thread, 0, detect_in_thread, 0)) error("Thread creation failed");

    //fetch_in_thread_sync(0); //fetch_in_thread(0);

    int count = 0;

/*
    if(!prefix && !dont_show){
        int full_screen = 0;
        create_window_cv("Demo", full_screen, 1352, 1013);
    }
*/


//    int send_http_post_once = 0;
//    const double start_time_lim = get_time_point();
    double before = get_time_point();
    double start_time = get_time_point();
    float avg_fps = 0;
    int frame_counter = 0;
    std::vector<float> fv;

    std::cout << "Inputs " <<  net.inputs   << std::endl;
    std::cout << "Outputs " <<  net.outputs << std::endl;

    while(datain && readjsonarray(datain, fv) ){
        ++count;

       // writejsonarray(std::cout, fv);

//        int outputs;
        if (net.inputs  > fv.size()) {
            std::cout << "Inputs mismatch" <<  net.inputs  << "/" << fv.size() << std::endl;
            break;
        }
        float *prediction = network_predict(net, fv.data());
        writejsonarray(dataout, prediction, net.outputs);
        writejsonarray(std::cout, prediction, net.outputs);

        //printf("\nFPS:%.1f \t AVG_FPS:%.1f\n", fps, avg_fps);

        /*
        if(count%1000==0) {
        //    delay = frame_skip;

            //double after = get_wall_time();
            //float curr = 1./(after - before);
            double after = get_time_point();    // more accurate time measurements
            float curr = 1000000. / (after - before);
            before = after;

            float spent_time = (get_time_point() - start_time) / 1000000;
            frame_counter++;
            if (spent_time >= 3.0f) {
                //printf(" spent_time = %f \n", spent_time);
                avg_fps = frame_counter / spent_time;
                frame_counter = 0;
                start_time = get_time_point();
            }

        }
         */
    }





//    free(avg);

   // free_ptrs((void **)names, net.layers[net.n - 1].classes);

    /*
    int i;
    const int nsize = 8;
    for (j = 0; j < nsize; ++j) {
        for (i = 32; i < 127; ++i) {
            free_image(alphabet[j][i]);
        }
        free(alphabet[j]);
    }
    free(alphabet);
    */

    free_network(net);

    //cudaProfilerStop();
}


