//**********************************//
//  ENCHANTER SYSTEM                //
//  Created by Gleb Papchihin       //
//  10/10/2020                      //
//**********************************//


#include <iostream>
#include <cmath>
using namespace std;


//**********************************//
//      Fundamental structs         //
//**********************************//

struct Point {
    float x = 0;
    float y = 0;

    Point add (Point b) const {
        return {x + b.x, y + b.y };
    }

    Point subtract (Point b) const {
        return {x - b.x, y - b.y };
    }

    float euclidean_distance(Point b = {0, 0}) const {
        float x_diff = x - b.x;
        float y_diff = y - b.y;
        return sqrt( pow(x_diff, 2) + pow(y_diff, 2) );
    }
};

struct Line {

    Line() = default;

    Line (Point new_start, Point new_end) {
        start = new_start;
        end   = new_end;
    }

    Point start;
    Point end;
};


//**********************************//
//      Classes                     //
//**********************************//

class  Plane {

public:

    Plane (int new_length) {

        if ( new_length < 1 )
            throw invalid_argument("Plane| Length must be more than 1");

        length      = new_length;
        points      = (Point *) malloc(length * sizeof(Point));
        distances   = (float *) malloc(length * sizeof(float));
    }

    void     set_point(Point new_point, int index) {
        check_index(index);
        points[index] = new_point;
    }

    int      get_length() const {
        return length;
    }

    Point    get_origin() const {
        return origin;
    }

    Point    get_point(int index, bool zero_origin = false) const {

        check_index(index);

        Point point  = points[index];

        if (zero_origin)
            point = point.add(origin);

        return point;
    }

    float    get_distance(int index) const {
        check_index(index);
        return distances[index];
    }

    void     create() {

        if (created) {
            cout << "The plane`s already created." << endl;
            return;
        }

        origin = find_origin();
        init_points();
        created = true;
    }

    ~Plane() {
        free(points);
        free(distances);
    }

private:

    int     length;
    Point   origin;
    Point   *points;
    float   *distances;
    bool    created = false;

    void     check_index( int index ) const {
        if ( (index >= length) || (index < 0) )
            throw invalid_argument("Plane| Element does not exist");
    }

    Point    find_origin() {

        Point point  = points[0];
        float max_x  = point.x;
        float max_y  = point.y;
        float min_x  = point.x;
        float min_y  = point.y;
        Point center;

        for (int i = 1; i < length; i++) {
            point = points[i];

            if (max_x < point.x)
                max_x = point.x;

            if (min_x > point.x)
                min_x = point.x;

            if (max_y < point.y)
                max_y = point.y;

            if (min_y > point.y)
                min_y = point.y;
        }

        center.x = (max_x - min_x) / 2 + min_x;
        center.y = (max_y - min_y) / 2 + min_y;

        return center;
    }

    void     init_points() {

        for (int i = 0; i < length; i++) {
            points[i]    = points[i].subtract(origin);
            distances[i] = points[i].euclidean_distance();
        }
    }
};

class  Map {

    /*
     * Description of class | Map is a unique
     * list of natural numbers, which
     * associated with indexes.
     * */

public:

    Map() = default;

    explicit    Map(Plane const &plane) {

        // Description | Create a list associated with
        //               all points on the plane.

        int plane_length = plane.get_length();

        for (int i = 0; i < plane_length; i++){
            append(i);
        }
    }

    void        append(int value) {

        check_value(value);

        length++;
        full_length++;
        values              = (int *)realloc(values, length*sizeof(int));
        values[length - 1]  = value;
    }

    int         get_length() const {
        return length;
    }

    void        insert_after(int index, int value) {

        check_index(index);
        check_value(value);

        int *new_values    = (int *)malloc((length + 1) * sizeof(int));
        int count          = 0;

        for (int i = 0; i < full_length; i++) {

            if (values[i] == -1)
                continue;

            new_values[count] = values[i];

            if (count == index) {
                count++;
                new_values[count] = value;
            }

            count++;
        }

        full_length = length + 1;
        length      = length + 1;
        values      = new_values;
    }

    void        remove_by_value(int value) {

        check_value(value);

        for (int i = 0; i < full_length; i++) {
            if (values[i] == value) {
                values[i] = -1;
                length--;
                break;
            }
        }
    }

    void        remove(int index) {

        check_index(index);

        int count = 0;
        for (int i = 0; i < full_length; i++) {
            if (values[i] != -1) {
                if (count == index) {
                    values[i] = -1;
                    break;
                }
                count++;
            }
        }

        length--;
    }

    int         get_value(int index) const {
        check_index(index);

        int count = 0;
        for (int i = 0; i < full_length; i++) {
            if (values[i] != -1) {
                if (count == index)
                    return values[i];
                count++;
            }
        }

        return -1;
    }

    ~Map() {
        free(values);
    }

private:

    int length      = 0;
    int full_length = 0;
    int *values     = (int *)malloc(length * sizeof(int));

    void        check_index( int index ) const {
        if ( (index >= length) || (index < 0) )
            throw invalid_argument("Map| Element does not exist");
    }

    void        check_value( int value ) {
        if (value < 0)
            throw invalid_argument("Map| Value must be more or equal to 0");
    }
};

class  Figure{

public:

    Figure(Plane const &plane, Map const &contour) {
        length = contour.get_length();
        origin = plane.get_origin();
        lines  = (Line *)malloc(length * sizeof(Line));

        if (length > 0)
            init_figure(plane, contour);
    }

    int     get_length () const {
        return length;
    }

    Line    get_line (int index, bool zero_origin = false) const {
        check_index(index);

        Line line = lines[index];

        if (zero_origin) {
            line.start = line.start.add(origin);
            line.end   = line.end.add(origin);
        }

        return line;
    }

    Point   get_origin() const {
        return origin;
    }

    Point   get_center_of_line(int index) const {
        check_index(index);

        Point center = find_center_of_line(index);
        return center;
    }

    Point   get_center_of_figure(bool zero_origin = false) const {
        Point center = center_of_figure;

        if ( zero_origin )
            center.add(origin);

        return center;
    }

    void    print_lines(bool zero_origin = false) const {

        cout << endl << "Figure: Number of lines is " << length << endl;

        for (int i = 0; i < length; i++) {

            Line line = get_line(i, zero_origin);
            cout << "| < " << line.start.x << "; " << line.start.y << " >" << endl;
        }

        cout << endl;
    }

    ~Figure() {
        free(lines);
    }

private:

    Line    *lines;
    int     length;
    Point   origin;
    Point   center_of_figure;

    void        check_index( int index ) const {
        if ( (index >= length) || (index < 0) )
            throw invalid_argument("Figure| Element does not exist");
    }

    Point       find_center_of_line(int index) const {

        Line  line      = get_line(index);
        Point start     = line.start;
        Point end       = line.end;
        float x_center  = (start.x + end.x) / 2;
        float y_center  = (start.y + end.y) / 2;
        Point center    = {x_center, y_center};
        return center;
    }

    void        init_figure(Plane const &plane, Map const &contour) {

        int     index_start;
        int     index_end;
        Point   start;
        Point   end;

        // Center of figure
        float   x_sum  = 0;
        float   y_sum  = 0;

        for (int i = 0; i < length; i++) {
            index_start     = contour.get_value(i);
            index_end       = (i == length - 1) ? contour.get_value(0) : contour.get_value(i + 1);
            start           = plane.get_point(index_start);
            end             = plane.get_point(index_end);
            lines[i]        = Line(start, end);

            // Center of a figure
            x_sum           += start.x;
            y_sum           += start.y;
        }

        // Center of a figure
        center_of_figure.x = x_sum / length;
        center_of_figure.y = y_sum / length;
    }
};


//**********************************//
//      Support functions           //
//**********************************//

Plane   create_plane() {
    int length;
    float x, y;

    cout << "Enter a length:";
    cin >> length;
    Plane plane(length);

    for (int i = 0; i < length; i++) {
        cout << "Enter x_" << i << ":";
        cin >> x;
        cout << "Enter y_" << i << ":";
        cin >> y;
        plane.set_point({x, y}, i);
    }

    plane.create();
    return plane;
}

Figure  create_figure(Plane const &plane, Map const &contour) {
    Figure figure(plane, contour);
    return figure;
}

Figure  create_figure_from_all_points(Plane const &plane) {
    Map     points(plane);
    Figure  figure(plane, points);
    return figure;
}

float   round(float value, int decimal_place) {

    if (decimal_place < 0)
        throw invalid_argument("Decimal place must be more than 0");

    int alpha = pow(10, decimal_place);
    return roundf(value * alpha) / alpha;
}

bool    does_point_belong_to_line(Line const &line, float x) {

    float max_x     = line.start.x >= line.end.x ? line.start.x : line.end.x;
    float min_x     = line.start.x <= line.end.x ? line.start.x : line.end.x;
    float rounded_x = round(x, 3);

    if ((max_x >= rounded_x) && (min_x <= rounded_x))
        return true;

    return false;
}


//**********************************//
//      Main functions              //
//**********************************//

int     get_index_of_farthest_point(Plane const &plane, Map const &unselected) {

    // Description| The function will return index of
    //              the farthest point on a plane.
    // Unselected | A list of indexes of points,
    //              That ain`t in a figure.

    int   max_index     = unselected.get_value(0);
    float max_distance  = plane.get_distance(max_index);
    int   length        = unselected.get_length();
    float distance;
    int   index;

    for (int i = 1; i < length; i++) {

        index       = unselected.get_value(i);
        distance    = plane.get_distance(index);

        if (distance > max_distance) {
            max_distance    = distance;
            max_index       = index;
        }
    }

    return max_index;
}

float   solve_linear_system(Line const &a, Line const &b) {
    Point shift_a       = a.start;
    Point shift_b       = b.start;
    Point eq_a          = a.end.subtract(shift_a);
    Point eq_b          = b.end.subtract(shift_b);
    float denominator   = (eq_a.y * eq_b.x) - (eq_a.x * eq_b.y);

    if (denominator == 0)
        throw invalid_argument("System has no or infinitely many solutions");

    float alpha         = (eq_a.y * shift_a.x * eq_b.x);
    float beta          = -(eq_b.y * shift_b.x * eq_a.x);
    float gamma         = (eq_a.x * eq_b.x) * (shift_b.y - shift_a.y);
    float nominator     = alpha + beta + gamma;
    float solution_on_x = nominator / denominator;

    return solution_on_x;
}

bool    are_lines_crossing(Line const &a, Line const &b) {

    try {
        float solution_on_x = solve_linear_system(a, b);
        bool  belong_to_a   = does_point_belong_to_line(a, solution_on_x);
        bool  belong_to_b   = does_point_belong_to_line(b, solution_on_x);

        if (belong_to_a && belong_to_b)
            return true;
    }
    catch (exception &e) {
        return true;
    }

    return false;
}

int     get_index_of_nearest_crossed_line(Point const &point, Figure const &figure) {

    // For distance
    int   min_index         = 0;
    int   length            = figure.get_length();
    Point nearest_center    = figure.get_center_of_line(min_index);
    float min_distance      = point.euclidean_distance(nearest_center);

    // For crossing
    bool  crossed           = false;
    Point center_of_figure  = figure.get_center_of_figure();
    Line  line_between_center_and_point(center_of_figure, point);

    for (int i = 0; i < length; i++) {

        Point center    = figure.get_center_of_line(i);
        float distance  = point.euclidean_distance(center);

        Line line       = figure.get_line(i);
        bool crossing   = are_lines_crossing(line_between_center_and_point, line);

        if ((distance <= min_distance) && crossing) {
            min_distance  = distance;
            crossed       = true;
            min_index     = i;
        }
    }

    if (crossed == false)
        return -1;

    return min_index;
}

bool    is_point_inside_figure(Point const &point, Figure const &figure) {

    int nearest_crossed_line = get_index_of_nearest_crossed_line(point, figure);

    if (nearest_crossed_line == -1)
        return true;

    return false;
}

Figure  enchanter(Plane const &plane) {

    if (plane.get_length() < 3)
        return create_figure_from_all_points(plane);

    Map unselected(plane);
    Map contour;

    int length      = unselected.get_length();

    for (int i = 0; i < length; i++) {

        int     farthest_index = get_index_of_farthest_point(plane, unselected);
        Point   farthest       = plane.get_point(farthest_index);
        Figure  figure         = create_figure(plane, contour);
        int nearest_line_index;

        if (figure.get_length() == 0) {
            unselected.remove_by_value(farthest_index);
            contour.append(farthest_index);
            continue;
        }

        if (is_point_inside_figure(farthest, figure)) {
            unselected.remove_by_value(farthest_index);
            continue;
        }

        nearest_line_index = get_index_of_nearest_crossed_line(farthest, figure);
        contour.insert_after(nearest_line_index, farthest_index);
        unselected.remove_by_value(farthest_index);
    }

    return create_figure(plane, contour);
}


int main() {

    Plane plane   = create_plane();
    Figure figure = enchanter(plane);
    figure.print_lines(true);

    return 0;
}
