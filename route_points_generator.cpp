#include "route_points_generator.h"

#include <algorithm>
#include <fstream>
#include <ros/package.h>

#include <boost/algorithm/string.hpp>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/filewritestream.h>

namespace aw
{

    const std::string kParkId(
        "/aw/planning/aw_planning/itinerary/park_id");
    const std::string kRouteId(
        "/aw/planning/aw_planning/itinerary/route_id");

    RoutePointsGenerator::RoutePointsGenerator(ros::NodeHandle &nh)
        : nh_(nh)
    {
        Init(nh);
    }

    void RoutePointsGenerator::Init(ros::NodeHandle &nh)
    {
        global_planning_ = std::make_unique<GlobalPlanning>(nh);
    }

    void RoutePointsGenerator::GenerateRoutePoints(void)
    {
        if (tasks_.empty())
        {
            return;
        }
        for (auto &tasks_ptr : tasks_)
        { //erase those point before start_point
            if (tasks_ptr.section_list.empty())
                continue;
            auto &section_front = tasks_ptr.section_list.front().lane_list.front();
            auto &section_back = tasks_ptr.section_list.back().lane_list.back();

            double Max = DBL_MAX;
            int max_index = 0;
            for (int i = 0; i < section_front.center_curve.size(); i++)
            {
                double x = section_front.center_curve[i].x - tasks_ptr.start_point.x;
                double y = section_front.center_curve[i].y - tasks_ptr.start_point.y;
                double Distance = sqrt(x * x + y * y);
                if (Distance < Max)
                {
                    Max = Distance;
                    max_index = i;
                }
            }
            section_front.center_curve.erase(section_front.center_curve.begin(),
                                             section_front.center_curve.begin() + max_index);
            Max = DBL_MAX;
            max_index = 0;
            for (int i = 0; i < section_back.center_curve.size(); i++)
            {
                double x = section_back.center_curve[i].x - tasks_ptr.end_point.x;
                double y = section_back.center_curve[i].y - tasks_ptr.end_point.y;
                double Distance = sqrt(x * x + y * y);
                if (Distance < Max)
                {
                    Max = Distance;
                    max_index = i;
                }
            }
            section_back.center_curve.erase(section_back.center_curve.begin() + max_index + 1,
                                            section_back.center_curve.end());
        }
        aw_idl::PlanningTask last_task;
        int i = 0;
        for (; i < tasks_.size(); i++)
        {
            if (!tasks_[i].section_list.empty())
            {
                last_task = tasks_[i];
                break;
            }
        }
        i++;
        for (; i < tasks_.size(); i++)
        {
            aw_idl::PlanningTask &task = tasks_[i];
            if (task.section_list.empty())
                continue;
            auto sect_ptr = task.section_list.begin();
            while (sect_ptr != task.section_list.end())
            { //erase those overlap in the middle
                if (sect_ptr->id == last_task.section_list.back().id)
                {
                    task.section_list.erase(task.section_list.begin(), sect_ptr);
                    break;
                }
                else
                    sect_ptr++;
            }
            //handle the cross section
            auto &cur_center_curve = task.section_list.begin()->lane_list.begin()->center_curve;
            auto &last_center_curve = last_task.section_list.back().lane_list.front().center_curve;
            auto begin_ptr = cur_center_curve.begin();
            auto last_ptr = last_center_curve.back();
            while (begin_ptr != cur_center_curve.end())
            {
                if (begin_ptr->x == last_ptr.x && begin_ptr->y == last_ptr.y)
                    break;
                else
                    begin_ptr++;
            }
            if (begin_ptr != cur_center_curve.end())
            { //those have overlap
                cur_center_curve.erase(cur_center_curve.begin(), begin_ptr);
                for (auto cet_ptr : cur_center_curve)
                {
                    last_center_curve.push_back(cet_ptr);
                }
            }
            else
            {
                auto cur_ptr = last_center_curve.begin();
                auto be_ptr = cur_center_curve.front();
                while (cur_ptr != last_center_curve.end())
                {
                    if (cur_ptr->x == be_ptr.x && cur_ptr->y == be_ptr.y)
                        break;
                    else
                        cur_ptr++;
                }
                if (cur_ptr != last_center_curve.end())
                { //fully coverd by last task
                    task.section_list.clear();
                }
                else
                { //non-overlap
                    for (auto center_point : cur_center_curve)
                    {
                        last_center_curve.push_back(center_point);
                    }
                }
            }
            //handle the extra
            if (task.section_list.size() > 1)
            {
                for (int k = 1; k < task.section_list.size(); k++)
                {
                    if (!task.section_list[k].lane_list.front().center_curve.empty())
                    {
                        last_task.section_list.push_back(task.section_list[k]);
                    }
                }
            }
        }
        return;
    }

    void RoutePointsGenerator::OutputRoutePoints()
    {
        std::string park_value_str;
        std::string route_value_str;
        ros::param::get(kParkId, park_value_str);
        ros::param::get(kRouteId, route_value_str);

        const std::string &package_path = ros::package::getPath("aw_global_planning");
        std::string filename = package_path + "/data/" + park_value_str + "_" + route_value_str + ".json";
        /**add**/
        std::string filename_without_suffix = package_path + "/data/" + park_value_str + "_" + route_value_str;
        /**add_end**/

        using namespace rapidjson;
        rapidjson::Document document;
        rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
        rapidjson::Value root(kObjectType);

        int index = 0;
        /**add**/
        int p2p_count = 0;
        /**add_end**/

        rapidjson::Value data(kArrayType);
        for (auto tasks_ptr : tasks_)
        {
            tasks_ptr.id = index;
            int task_id = tasks_ptr.id;
            index++;
            rapidjson::Value points(kArrayType);
            rapidjson::Value task_json(kObjectType);
            task_json.AddMember("index", task_id, allocator);
            if (tasks_ptr.type == aw_idl::PlanningTask::ASTAR)
            {
                /**add**/
                if (task_id == p2p_count)
                {
                    p2p_count += 1;
                    // data.PushBack(task_json, allocator);
                    continue;
                }
                /**add_end**/
                rapidjson::Value point_start(kArrayType);
                rapidjson::Value point_end(kArrayType);
                point_start.PushBack(tasks_ptr.start_point.x, allocator);
                point_start.PushBack(tasks_ptr.start_point.y, allocator);
                point_end.PushBack(tasks_ptr.end_point.x, allocator);
                point_end.PushBack(tasks_ptr.end_point.y, allocator);
                points.PushBack(point_start, allocator);
                points.PushBack(point_end, allocator);
                task_json.AddMember("points", points, allocator);
                task_json.AddMember("type", "p2p", allocator);
                data.PushBack(task_json, allocator);
            }
            else if (tasks_ptr.type == aw_idl::PlanningTask::STRUCTED_MAP)
            {
                for (auto section_ptr : tasks_ptr.section_list)
                {
                    for (auto lane_ptr : section_ptr.lane_list)
                    {
                        for (auto center_curve_ptr : lane_ptr.center_curve)
                        {
                            rapidjson::Value point(kArrayType);
                            point.PushBack(center_curve_ptr.x, allocator);
                            point.PushBack(center_curve_ptr.y, allocator);
                            points.PushBack(point, allocator);
                        }
                    }
                }
                task_json.AddMember("points", points, allocator);
                task_json.AddMember("type", "map", allocator);
                data.PushBack(task_json, allocator);
            }
            data.PushBack(task_json, allocator);
        }
        root.AddMember("data", data, allocator);
        rapidjson::Value park_value;
        rapidjson::Value route_value;
        park_value.SetString(rapidjson::StringRef(park_value_str.c_str()));
        route_value.SetString(rapidjson::StringRef(route_value_str.c_str()));

        root.AddMember("park_id", park_value, allocator);
        root.AddMember("route_id", route_value, allocator);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        root.Accept(writer);
        /**add**/
        std::ifstream checkfs(filename);
        const std::string temp_str("_new");
        while (checkfs)
        {
            checkfs.close();
            checkfs.clear();
            filename_without_suffix.append(temp_str);
            filename = filename_without_suffix + ".json";
            checkfs.open(filename);
        }
        /**add_end**/
        std::ofstream os(filename);
        if (!os.is_open())
        {
            ROS_ERROR("Cannot open or create [%s].", filename.c_str());
        }
        else
        {
            os << buffer.GetString();
        }
        os.close();
        return;
    }

    void RoutePointsGenerator::GenerateRoute()
    {
        global_planning_->LoadTasks();
        global_planning_->StaticTasksReady();
        this->tasks_ = global_planning_->GetTasks();
        GenerateRoutePoints();
        OutputRoutePoints();
    }

} // namespace aw

int main(int argc, char **argv)
{
    ros::init(argc, argv, "route_points_generator");
    ros::NodeHandle nh;

    aw::RoutePointsGenerator planning(nh);
    planning.GenerateRoute();
    ros::shutdown();

    return 0;
}
