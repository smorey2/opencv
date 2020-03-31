// https://rdmilligan.wordpress.com/2015/07/19/glyph-recognition-using-opencv-and-python/
#include <opencv2/opencv.hpp>
#include <iostream>
#include <xtensor/xtensor.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xsort.hpp>

//using namespace cv;
using namespace std;

cv::RNG _rng(12345);

// https://www.reddit.com/r/cpp/comments/9d1w3o/c_implementation_of_the_python_numpy_library/
// https://xtensor.readthedocs.io/en/latest/numpy.html
// https://www.youtube.com/watch?v=LYeJ2FUYiqA

//constexpr bool operator< (const cv::Point &left, const cv::Point &right)
//{
//	return true;
//}


xt::xtensor<cv::Point2f, 4> orderPoints(xt::xtensor<cv::Point, 4> points)
{
	auto s = xt::sum(points, 1);
	auto d = xt::diff(points, 1, 1);

	return xt::xarray<cv::Point2f>();


	//return xt::xarray<cv::Point2f>({
	//	points[xt::argmin(s)],
	//	points[xt::argmin(d)],
	//	points[xt::argmax(s)],
	//	points[xt::argmax(d)] });
}

cv::Size maxWidthHeight(xt::xtensor<cv::Point, 4> points)
{
	auto tl = points[0], tr = points[1], br = points[2], bl = points[4];
	auto topWidth = sqrt(((tr.x - tl.x) ^ 2) + ((tr.y - tl.y) ^ 2));
	auto bottomWidth = sqrt(((br.x - bl.x) ^ 2) + ((br.y - bl.y) ^ 2));
	auto maxWidth = max((int)topWidth, (int)bottomWidth);
	auto leftHeight = sqrt(((tl.x - bl.x) ^ 2) + ((tl.y - bl.y) ^ 2));
	auto rightHeight = sqrt(((tr.x - br.x) ^ 2) + ((tr.y - br.y) ^ 2));
	auto maxHeight = max((int)leftHeight, (int)rightHeight);
	return { maxWidth, maxHeight };
}

xt::xtensor<cv::Point2f, 4> topdownPoints(int maxWidth, int maxHeight)
{
	return xt::xarray<cv::Point2f>({
		{0.f, 0.f},
		{maxWidth - 1.f, 0.f},
		{maxWidth - 1.f, maxHeight - 1.f},
		{0.f, maxHeight - 1.f} });
}

void getTopdownQuad(cv::InputArray image, cv::OutputArray warped, xt::xtensor<cv::Point2f, 4> src)
{
	// src and dst points
	src = orderPoints(src);

	auto size = maxWidthHeight(src);
	auto dst = topdownPoints(size.width, size.height);

	// warp perspective
	auto matrix = cv::getPerspectiveTransform(src.data(), dst.data());
	cv::warpPerspective(image, warped, matrix, maxWidthHeight(src));
}

/*

def addSubstituteQuad(image, substituteQuad, dst)
{

	// dst (zero-set) and src points
	dst = order_points(dst)

	(tl, tr, br, bl) = dst
	min_x = min(int(tl[0]), int(bl[0]))
	min_y = min(int(tl[1]), int(tr[1]))

	for point in dst:
		point[0] = point[0] - min_x
		point[1] = point[1] - min_y

	(max_width,max_height) = max_width_height(dst)
	src = topdown_points(max_width, max_height)

	// warp perspective (with white border)
	substitute_quad = cv2.resize(substitute_quad, (max_width,max_height))

	warped = np.zeros((max_height,max_width, 3), np.uint8)
	warped[:,:,:] = 255

	matrix = cv2.getPerspectiveTransform(src, dst)
	cv2.warpPerspective(substitute_quad, matrix, (max_width,max_height), warped, borderMode=cv2.BORDER_TRANSPARENT)

	// add substitute quad
	image[min_y:min_y + max_height, min_x:min_x + max_width] = warped

	return image
}

def getGlyphPattern(image, blackThreshold, whiteThreshold)
{
	// collect pixel from each cell (left to right, top to bottom)
	cells = []

	cell_half_width = int(round(image.shape[1] / 10.0))
	cell_half_height = int(round(image.shape[0] / 10.0))

	row1 = cell_half_height*3
	row2 = cell_half_height*5
	row3 = cell_half_height*7
	col1 = cell_half_width*3
	col2 = cell_half_width*5
	col3 = cell_half_width*7

	cells.append(image[row1, col1])
	cells.append(image[row1, col2])
	cells.append(image[row1, col3])
	cells.append(image[row2, col1])
	cells.append(image[row2, col2])
	cells.append(image[row2, col3])
	cells.append(image[row3, col1])
	cells.append(image[row3, col2])
	cells.append(image[row3, col3])

	// threshold pixels to either black or white
	for idx, val in enumerate(cells):
		if val < black_threshold:
			cells[idx] = 0
		elif val > white_threshold:
			cells[idx] = 1
		else:
			return None

	return cells
}

def resizeImage(image, newSize)
{
	ratio = newSize / image.shape[1]
	return cv::resize(image,((int)newSize,(int)image.shape[0]*ratio));
}

def rotateImage(image, angle)
{
	(h, w) = image.shape[:2]
	center = (w / 2, h / 2)
	rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0)
	return cv::warpAffine(image, rotationMatrix, (w, h))
}

*/

int main()
{
	cv::namedWindow("main", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("drawing", cv::WINDOW_AUTOSIZE);

	cv::VideoCapture cap;
	cap.open(0);

	if (!cap.isOpened())
	{
		std::cerr << "Couldn't open capture." << std::endl;
		return -1;
	}

	cv::UMat frame, gray, edges;
	vector<vector<cv::Point>> contours;

	for (;;)
	{
		cap >> frame;
		if (frame.empty()) break;

		cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
		cv::GaussianBlur(gray, gray, { 5, 5 }, 0);
		cv::Canny(gray, edges, 100, 200);
		cv::imshow("main", edges);

		// Find contours
		cv::findContours(edges, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

		// Draw contours
		cv::Mat drawing = cv::Mat::zeros(edges.size(), CV_8UC3);

		cv::Mat warped;
		vector<cv::Point> contour, approx;
		std::vector<std::size_t> shape = { 4 };
		for (auto i = 0; i < contours.size(); i++)
		{
			contour = contours[i];
			auto perimeter = cv::arcLength(contour, true);

			cv::approxPolyDP(contour, approx, 0.01*perimeter, true);
			if (approx.size() != 4)
				continue;
			auto approx2 = xt::adapt(approx, shape);
			getTopdownQuad(gray, warped, approx2);

			cv::Scalar color = cv::Scalar(_rng.uniform(0, 255), _rng.uniform(0, 255), _rng.uniform(0, 255));
			cv::drawContours(drawing, contours, i, color, 2);
		}
		cv::imshow("drawing", drawing);

		auto c = cv::waitKey(10);
		if (c == 27) break;
	}

	cap.release();
	return 0;
}