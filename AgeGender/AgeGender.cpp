#include <tuple>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iterator>

using namespace cv;
using namespace cv::dnn;
using namespace std;

/**

 * @brief Get the Face Box object ������λ
 *
 * @param net �����������
 * @param frame ���ͼ��
 * @param conf_threshold ��ֵ
 * @return tuple<Mat, vector<vector<int>>> Ԫ���������ɷ��ض��ֵ
 */

tuple<Mat, vector<vector<int>>> getFaceBox(Net net, Mat& frame, double conf_threshold)

{
	//ͼ����
	Mat frameOpenCVDNN = frame.clone();
	int frameHeight = frameOpenCVDNN.rows;
	int frameWidth = frameOpenCVDNN.cols;
	//���ųߴ�
	double inScaleFactor = 1.0;
	//���ͼ��С
	Size size = Size(300, 300);
	// std::vector<int> meanVal = {104, 117, 123};
	Scalar meanVal = Scalar(104, 117, 123);
	cv::Mat inputBlob;
	inputBlob = cv::dnn::blobFromImage(frameOpenCVDNN, inScaleFactor, size, meanVal, true, false);
	net.setInput(inputBlob, "data");
	//��ά�������
	cv::Mat detection = net.forward("detection_out");
	//��ȡ�����Ϣ
	cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
	vector<vector<int>> bboxes;
	for (int i = 0; i < detectionMat.rows; i++)
	{
		//Ԥ�����
		float confidence = detectionMat.at<float>(i, 2);
		if (confidence > conf_threshold)
		{
			//���Ͻǵ㣬���걻��һ��
			int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frameWidth);
			int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frameHeight);
			//���½ǽǵ㣬���걻��һ��
			int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frameWidth);
			int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frameHeight);
			vector<int> box = { x1, y1, x2, y2 };
			//��������
			bboxes.push_back(box);
			//ͼ���ѡ
			cv::rectangle(frameOpenCVDNN, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2, 4);
		}
	}
	return make_tuple(frameOpenCVDNN, bboxes);
}

int main(void)
{
	//����ģ��
	string faceProto = "../model/opencv_face_detector.pbtxt";
	string faceModel = "../model/opencv_face_detector_uint8.pb";
	//����ģ��
	string ageProto = "../model/age_deploy.prototxt";
	string ageModel = "../model/age_net.caffemodel";
	//�Ա�ģ��
	string genderProto = "../model/gender_deploy.prototxt";
	string genderModel = "../model/gender_net.caffemodel";
	//��ֵ
	Scalar MODEL_MEAN_VALUES = Scalar(78.4263377603, 87.7689143744, 114.895847746);
	//����α�ǩ
	vector<string> ageList = { "(0-2)", "(4-6)", "(8-12)", "(15-20)", "(25-32)",
		"(38-43)", "(48-53)", "(60-100)" };

	//�Ա��ǩ
	vector<string> genderList = { "Male", "Female" };
	//��������
	Net ageNet = cv::dnn::readNet(ageProto, ageModel);
	Net genderNet = cv::dnn::readNet(genderProto, genderModel);
	Net faceNet = cv::dnn::readNetFromTensorflow(faceModel, faceProto);
	//������ͷ
	VideoCapture cap;
	cap.open(0);
	if (cap.isOpened())
	{
		cout << "camera is opened!" << endl;
	}
	else
	{
		return 0;
	}
	int padding = 20;
	while (waitKey(1) < 0)
	{
		// read frame ��ͼ
		Mat frame;
		cap.read(frame);
		if (frame.empty())
		{
			waitKey();
			break;
		}
		frame = imread("./image/jolie.jpg");
		//��������
		vector<vector<int>> bboxes;
		//���������ͼ
		Mat frameFace;
		//������λ
		//tie()�������frameFace��bboxes
		tie(frameFace, bboxes) = getFaceBox(faceNet, frame, 0.7);
		//�����ж�
		if (bboxes.size() == 0)
		{
			cout << "No face detected, checking next frame." << endl;
			continue;
		}
		//�����ȡ�������
		for (auto it = begin(bboxes); it != end(bboxes); ++it)
		{
			//��ѡ����
			Rect rec(it->at(0) - padding, it->at(1) - padding, it->at(2) - it->at(0) + 2 * padding, it->at(3) - it->at(1) + 2 * padding);
			//����������ѡ����ͼ���Ե
			rec.width = ((rec.x + rec.width) > frame.cols) ? (frame.cols - rec.x - 1) : rec.width;
			rec.height = ((rec.y + rec.height) > frame.rows) ? (frame.rows - rec.y - 1) : rec.height;
			// take the ROI of box on the frame,ԭͼ����ȡ����
			Mat face = frame(rec);
			//�Ա���
			Mat blob;
			blob = blobFromImage(face, 1, Size(227, 227), MODEL_MEAN_VALUES, false);
			genderNet.setInput(blob);
			// string gender_preds; ��ȡǰ�򴫲�softmax���
			vector<float> genderPreds = genderNet.forward();
			// find max element index max_element������Ѱ���ֵ
			// distance function does the argmax() work in C++ distance�������ֵ�͵�һ��ֵ�±�ľ���
			int max_index_gender = std::distance(genderPreds.begin(), max_element(genderPreds.begin(), genderPreds.end()));
			//��ü����
			string gender = genderList[max_index_gender];
			cout << "Gender: " << gender << endl;
			//����ʶ��
			ageNet.setInput(blob);
			vector<float> agePreds = ageNet.forward();
			// finding maximum indicd in the age_preds vector �ҵ�����Ԥ������±�
			int max_indice_age = std::distance(agePreds.begin(), max_element(agePreds.begin(), agePreds.end()));
			string age = ageList[max_indice_age];
			cout << "Age: " << age << endl;
			// label �����ǩ
			string label = gender + ", " + age;
			//��������λͼ����ʾ���
			cv::putText(frameFace, label, Point(it->at(0), it->at(1) - 15), cv::FONT_HERSHEY_SIMPLEX, 0.9, Scalar(0, 255, 255), 2, cv::LINE_AA);
		}
		//������
		imshow("Frame", frameFace);
		imwrite("./image/out.jpg", frameFace);
	}
}