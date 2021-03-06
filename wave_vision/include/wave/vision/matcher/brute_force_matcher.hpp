/**
 * @file
 * Brute force matcher implementation, derived from descriptor matcher base
 * class.
 * @ingroup vision
 */
#ifndef WAVE_VISION_BRUTE_FORCE_MATCHER_HPP
#define WAVE_VISION_BRUTE_FORCE_MATCHER_HPP

#include <string>
#include <vector>

#include "wave/vision/matcher/descriptor_matcher.hpp"

namespace wave {
/** @addtogroup vision
 *  @{ */

/** Configuration parameters for the BruteForceMatcher */
struct BFMatcherParams {
    BFMatcherParams() {}

    /** Constructor with user selected values. Only to be used if the user
     *  desires the ratio test as the first pass to filter outliers.
     */
    BFMatcherParams(int norm_type,
                    double ratio_threshold,
                    bool auto_remove_outliers,
                    int fm_method)
        : norm_type(norm_type),
          use_knn(true),
          ratio_threshold(ratio_threshold),
          auto_remove_outliers(auto_remove_outliers),
          fm_method(fm_method) {}

    /** Overloaded method. Only to be used if the user desires the distance
     *  threshold test as the first pass to filter outliers.
     */
    BFMatcherParams(int norm_type,
                    int distance_threshold,
                    bool auto_remove_outliers,
                    int fm_method)
        : norm_type(norm_type),
          use_knn(false),
          distance_threshold(distance_threshold),
          auto_remove_outliers(auto_remove_outliers),
          fm_method(fm_method) {}

    /** Constructor using parameters extracted from a configuration file.
     *
     *  @param config_path the path to the location of the configuration file
     */
    BFMatcherParams(const std::string &config_path);

    /** Norm type to use for distance calculation between feature descriptors.
     *
     *  Options:
     *  cv::NORM_INF: l-infinity norm
     *  cv::NORM_L1: l1 norm
     *  cv::NORM_L2: l2 norm
     *  cv::NORM_L2SQR: l2 norm, squared
     *  cv::NORM_HAMMING: Hamming distance
     *  cv::NORM_HAMMING2: Hamming distance modifier for ORB Descriptor, with
     *  tuple_size (WTA_K) = 3 or 4
     *
     *  As per OpenCV docs, NORM_L1 and NORM_L2 is valid for the SIFT or
     *  SURF descriptors, while NORM_HAMMING is valid for the ORB, BRISK, and
     *  BRIEF descriptors. NORM_HAMMING2 should only be used with ORB when
     *  tuple_size (WTA_K) is 3 or 4.
     *
     *  Default: NORM_HAMMING, as it is used for ORB and BRISK Descriptors.
     *
     *  Please refer to further description of the norm types
     *  [here][opencv_norm_types].
     *
     *  [opencv_norm_types]:
     *  http://docs.opencv.org/trunk/d2/de8/group__core__array.html#gad12cefbcb5291cf958a85b4b67b6149f
     */
    int norm_type = cv::NORM_HAMMING;

    /** Determines whether to use a k-nearest-neighbours match.
     *
     *  Matcher can conduct a knn match with the best 2 matches for each
     *  descriptor. This uses the ratio test (@param ratio_threshold)
     *  to discard outliers.
     *
     *  If false, the matcher uses a distance heuristic
     *  (@param distance_threshold) to discard poor matches. This also
     *  incorporates cross checking between matches.
     *
     *  Recommended: true.
     */
    bool use_knn = true;

    /** Specifies heuristic for the ratio test, illustrated by Dr. David G. Lowe
     *  in his paper _Distinctive Image Features from Scale-Invariant Keypoints_
     *  (2004). The test takes the ratio of the closest keypoint distance
     *  to that of the second closest neighbour. If the ratio is less than
     *  the heuristic, it is discarded.
     *
     *  A value of 0.8 was shown by Dr. Lowe to reject 90% of the false matches,
     *  and discard only 5% of the correct matches.
     *
     *  Recommended: 0.8. Must be between 0 and 1.
     */
    double ratio_threshold = 0.8;

    /** Specifies the distance threshold for good matches.
     *
     *  Matches will only be kept if the descriptor distance is less than or
     *  equal to the product of the distance threshold and the _minimum_ of all
     *  descriptor distances. The greater the value, the more matches will
     *  be kept.
     *
     *  Recommended: 5. Must be greater than or equal to zero.
     */
    int distance_threshold = 5;

    /** Determines whether to automatically remove outliers using the method
     *  described in fm_method.
     *
     *  If true, the wave::BruteForceMatcher::matchDescriptors method will
     *  automatically call the wave::BruteForceMatcher::removeOutliers method,
     *  which uses the method in wave::BFMatcherParams::fm_method to remove
     *  matched outliers.
     *
     *  If false, the matches returned will only have been passed through the
     *  distance threshold or ratio tests described above.
     *
     *  Recommended: True
     */
    bool auto_remove_outliers = true;

    /** Method to find the fundamental matrix and remove outliers.
     *
     *  Options:
     *  cv::FM_7POINT: 7-point algorithm
     *  cv::FM_8POINT: 8-point algorithm
     *  cv::FM_LMEDS : least-median algorithm
     *  cv::FM_RANSAC: RANSAC algorithm
     *
     *  Recommended: cv::FM_RANSAC.
     */
    int fm_method = cv::FM_RANSAC;
};

/** Representation of a descriptor matcher using the BruteForce algorithm.
 *
 *  Internally, this class is wrapping OpenCV's BFMatcher module.
 *  Further reference on the BFMatcher can be found
 * [here][opencv_bfmatcher].
 *
 *  [opencv_bfmatcher]:
 *  http://docs.opencv.org/trunk/d3/da1/classcv_1_1BFMatcher.html
 */
class BruteForceMatcher : public DescriptorMatcher {
 public:
    /** Default constructor. The user can also specify their own struct with
     *  desired values. If no struct is provided, default values are used.
     *
     *  @param config contains the desired parameter values.
     */
    explicit BruteForceMatcher(
      const BFMatcherParams &config = BFMatcherParams{});

    /** Returns the current configuration parameters being used by the
     *  BruteForceMatcher
     *
     *  @return the current configuration values.
     */
    BFMatcherParams getConfiguration() const {
        return this->current_config;
    }

    /** Remove outliers between matches using epipolar constraints
     *
     * @param matches the unfiltered matches computed from two images
     * @param keypoints_1 the keypoints from the first image
     * @param keypoints_2 the keypoints from the second image
     *
     * @return the filtered matches
     */
    std::vector<cv::DMatch> removeOutliers(
      const std::vector<cv::DMatch> &matches,
      const std::vector<cv::KeyPoint> &keypoints_1,
      const std::vector<cv::KeyPoint> &keypoints_2) const override;

    /** Matches keypoints descriptors between two images using the
     *  BruteForceMatcher.
     *
     *  @param descriptors_1 the descriptors extracted from the first image.
     *  @param descriptors_2 the descriptors extracted from the second image.
     *  @param keypoints_1 the keypoints detected in the first image
     *  @param keypoints_2 the keypoints detected in the second image
     *  @param mask
     *  \parblock indicates which descriptors can be matched between the two
     *  sets. As per OpenCV docs "queryDescriptors[i] can be matched with
     *  trainDescriptors[j] only if masks.at<uchar>(i,j) is non-zero. In the
     *  libwave wrapper, queryDescriptors are descriptors_1, and
     *  trainDescriptors are descriptors_2. Default is cv::noArray().
     *  \endparblock
     *
     *  @return vector containing the best matches.
     */
    std::vector<cv::DMatch> matchDescriptors(
      cv::Mat &descriptors_1,
      cv::Mat &descriptors_2,
      const std::vector<cv::KeyPoint> &keypoints_1,
      const std::vector<cv::KeyPoint> &keypoints_2,
      cv::InputArray mask = cv::noArray()) const override;

 private:
    /** The pointer to the wrapped cv::BFMatcher object */
    cv::Ptr<cv::BFMatcher> brute_force_matcher;

    /** Current configuration parameters */
    BFMatcherParams current_config;

    /** Remove outliers between matches. Uses a heuristic based approach as a
     *  first pass to determine good matches.
     *
     *  @param matches the unfiltered matches computed from two images.
     *
     *  @return the filtered matches.
     */
    std::vector<cv::DMatch> filterMatches(
      const std::vector<cv::DMatch> &matches) const override;

    /** Overloaded method, which takes in a vector of a vector of matches. This
     *  is designed to be used with the knnMatchDescriptors method, and uses the
     *  ratio test to filter the matches.
     *
     *  @param matches the unfiltered matches computed from two images.
     *
     *  @return the filtered matches.
     */
    std::vector<cv::DMatch> filterMatches(
      const std::vector<std::vector<cv::DMatch>> &matches) const override;


    /** Checks whether the desired configuration is valid
     *
     *  @param check_config containing the desired configuration values.
     */
    void checkConfiguration(const BFMatcherParams &check_config) const;
};
/** @} end of group */
}  // namespace wave

#endif  // WAVE_VISION_BRUTE_FORCE_MATCHER_HPP
