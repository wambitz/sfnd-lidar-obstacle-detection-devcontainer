#include "processPointClouds.h"
#include "render/render.h"
#include <random>
#include <unordered_set>
// using templates for processPointClouds so also include .cpp to help linker
#include "processPointClouds.cpp"

using render::Color;
using render::renderPointCloud;

pcl::PointCloud<pcl::PointXYZ>::Ptr CreateData()
{
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>());
    // Add inliers
    float scatter = 0.6;
    for (int i = -5; i < 5; i++)
    {
        double rx = 2 * (((double)rand() / (RAND_MAX)) - 0.5);
        double ry = 2 * (((double)rand() / (RAND_MAX)) - 0.5);
        pcl::PointXYZ point;
        point.x = i + scatter * rx;
        point.y = i + scatter * ry;
        point.z = 0;

        cloud->points.push_back(point);
    }
    // Add outliers
    int numOutliers = 10;
    while (numOutliers--)
    {
        double rx = 2 * (((double)rand() / (RAND_MAX)) - 0.5);
        double ry = 2 * (((double)rand() / (RAND_MAX)) - 0.5);
        pcl::PointXYZ point;
        point.x = 5 * rx;
        point.y = 5 * ry;
        point.z = 0;

        cloud->points.push_back(point);
    }
    cloud->width = cloud->points.size();
    cloud->height = 1;

    return cloud;
}

pcl::PointCloud<pcl::PointXYZ>::Ptr CreateData3D()
{
    ProcessPointClouds<pcl::PointXYZ> pointProcessor;
    return pointProcessor.loadPcd("../../../sensors/data/pcd/simpleHighway.pcd");
}

pcl::visualization::PCLVisualizer::Ptr initScene()
{
    pcl::visualization::PCLVisualizer::Ptr viewer(new pcl::visualization::PCLVisualizer("2D Viewer"));
    viewer->setBackgroundColor(0, 0, 0);
    viewer->initCameraParameters();
    viewer->setCameraPosition(0, 0, 15, 0, 1, 0);
    viewer->addCoordinateSystem(1.0);
    return viewer;
}

// UDACITY Implementation
// std::unordered_set<int> RansacPlane(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, int maxIterations, float
// distanceTolerance)
// {
//     auto startTime = std::chrono::steady_clock::now();

//     std::unordered_set<int> inliersResult;
//     srand(time(NULL));

//     /*
//     TODO: Fill in this function
//     For max iterations:
//     1. Randomly sample subset and fit line
//     2. Measure distance between every point and fitted line
//        If distance is smaller than threshold count it as inlier
//     3. Return indicies of inliers from fitted line with most inliers
//     */

//     while (maxIterations--)
//     {
//         std::unordered_set<int> inliers;
//         while (inliers.size() < 3)
//         {
//             inliers.insert(rand() % cloud->points.size());
//         }

//         auto itr = inliers.begin();
//         float x1 = cloud->points[*itr].x;
//         float y1 = cloud->points[*itr].y;
//         float z1 = cloud->points[*itr].z;

//         itr++;

//         float x2 = cloud->points[*itr].x;
//         float y2 = cloud->points[*itr].y;
//         float z2 = cloud->points[*itr].z;

//         itr++;

//         float x3 = cloud->points[*itr].x;
//         float y3 = cloud->points[*itr].y;
//         float z3 = cloud->points[*itr].z;

//         float a = (y2 - y1) * (z3 - z1) - (z2 - z1) * (y3 - y1); // i
//         float b = (z2 - z1) * (x3 - x1) - (x2 - x1) * (z3 - z1); // j
//         float c = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1); // k
//         float d = - (a * x1 + b * y1 + c * z1);

//         for (int index = 0; index < cloud->points.size(); index++)
//         {
//             if (inliers.count(index) > 0)
//             {
//                 continue;
//             }

//             pcl::PointXYZ point = cloud->points[index];
//             float x4 = point.x;
//             float y4 = point.y;
//             float z4 = point.z;

//             float distance = fabs(a * x4 + b * y4 + c * z4 + d) / sqrt(a * a + b * b + c * c);

//             if (distance <= distanceTolerance )
//             {
//                 inliers.insert(index);
//             }
//         }

//         if (inliers.size() > inliersResult.size())
//         {
//             inliersResult = inliers;
//         }
//     }
//     auto endTime = std::chrono::steady_clock::now();
//     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

//     return inliersResult;
// }

std::unordered_set<int> Ransac(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, int maxIterations, float distanceTolerance)
{
    auto startTime = std::chrono::steady_clock::now();

    std::unordered_set<int> inliersResult;

    // Use modern C++ random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, cloud->points.size() - 1);

    while (maxIterations--)
    {
        std::unordered_set<int> inliers;
        while (inliers.size() < 2)
        {
            inliers.insert(dis(gen));
        }

        auto itr = inliers.begin();
        float x1 = cloud->points[*itr].x;
        float y1 = cloud->points[*itr].y;

        itr++;
        float x2 = cloud->points[*itr].x;
        float y2 = cloud->points[*itr].y;

        float a = (y1 - y2);
        float b = (x2 - x1);
        float c = (x1 * y2 - x2 * y1);

        for (int index = 0; index < cloud->points.size(); ++index)
        {
            if (inliers.count(index) > 0)
            {
                continue;
            }

            auto& point = cloud->points[index];
            float x3 = point.x;
            float y3 = point.y;

            float distance = std::fabs(a * x3 + b * y3 + c) / std::sqrt(a * a + b * b);

            if (distance <= distanceTolerance)
            {
                inliers.insert(index);
            }
        }

        if (inliers.size() > inliersResult.size())
        {
            inliersResult = inliers;
        }
    }

    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "RANSAC Duration: " << duration.count() << " milliseconds" << std::endl;

    return inliersResult;
}

int main()
{

    // Create viewer
    pcl::visualization::PCLVisualizer::Ptr viewer = initScene();

    // Create data
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud = CreateData3D();

    // TODO: Change the max iteration and distance tolerance arguments for Ransac function
    std::unordered_set<int> inliers = RansacPlane(cloud, 10, 0.5);

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloudInliers(new pcl::PointCloud<pcl::PointXYZ>());
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloudOutliers(new pcl::PointCloud<pcl::PointXYZ>());

    for (int index = 0; index < cloud->points.size(); index++)
    {
        pcl::PointXYZ point = cloud->points[index];
        if (inliers.count(index))
            cloudInliers->points.push_back(point);
        else
            cloudOutliers->points.push_back(point);
    }

    // Render 2D point cloud with inliers and outliers
    if (inliers.size())
    {
        renderPointCloud(viewer, cloudInliers, "inliers", Color(0, 1, 0));
        renderPointCloud(viewer, cloudOutliers, "outliers", Color(1, 0, 0));
    }
    else
    {
        renderPointCloud(viewer, cloud, "data");
    }

    while (!viewer->wasStopped())
    {
        viewer->spinOnce();
    }
}
