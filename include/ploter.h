using namespace vex;

#include <vector>
#include <limits>
#include <algorithm>

//-----------------------------------------------------------
// XYPlotter Class: Draws an X/Y plot for two data series on VEX V5 Brain's screen.
//-----------------------------------------------------------
class XYPlotter {
public:
  // Constructor sets screen size and margin parameters.
  XYPlotter()
    : screenWidth(480),
      screenHeight(240),
      marginLeft(5),
      marginRight(5),
      marginTop(5),
      marginBottom(5),
      minX(0),
      maxX(100),
      minY(0),
      maxY(100)
  { }

  // Add a data point to series 1 (plotted in red)
  void addDataPointSeries1(double x, double y) {
    s1x.push_back(x);
    s1y.push_back(y);
  }

  // Add a data point to series 2 (plotted in blue)
  void addDataPointSeries2(double x, double y) {
    s2x.push_back(x);
    s2y.push_back(y);
  }

// Add a data point to series 3 (plotted in green)
  void addDataPointSeries3(double x, double y) {
    s3x.push_back(x);
    s3y.push_back(y);
  }

  // Clear both data series
  void clearData() {
    s1x.clear(); s1y.clear();
    s2x.clear(); s2y.clear();
    s3x.clear(); s3y.clear();
  }

  // Draw the plot: clear the screen, draw the boundary, and plot both series.
  void drawPlot() {
    // Recalculate plot boundaries based on current data.
    updateLimits();

    // Clear the screen.
    Brain.Screen.clearScreen();

    // Define the plotting area using the margins.
    int plotX      = marginLeft;
    int plotY      = marginTop;
    int plotWidth  = screenWidth - marginLeft - marginRight;
    int plotHeight = screenHeight - marginTop - marginBottom;

    // Draw a border around the plotting area.
    Brain.Screen.drawRectangle(plotX, plotY, plotWidth, plotHeight, vex::color::black);

    // Plot series 1 in red.
    drawSeries(s1x, s1y, vex::color::red);

    // Plot series 2 in blue.
    drawSeries(s2x, s2y, vex::color::blue);

    // Plot series 2 in blue.
    drawSeries(s3x, s3y, vex::color::green);

    // Optionally: print the ranges on the screen.
    // Brain.Screen.printAt(plotX, plotY + 20, "X: %.2f to %.2f", minX, maxX);
    // Brain.Screen.printAt(plotX, plotY + 40, "Y: %.2f to %.2f", minY, maxY);
  }

private:
  // Vectors storing the data for each series.
  std::vector<double> s1x, s1y;
  std::vector<double> s2x, s2y;
  std::vector<double> s3x, s3y;
  
  // Data range values.
  double minX, maxX, minY, maxY;

  // Screen dimensions and margins.
  const int screenWidth;
  const int screenHeight;
  const int marginLeft;
  const int marginRight;
  const int marginTop;
  const int marginBottom;

  // Recalculate the minimum and maximum values for X and Y from the data.
  void updateLimits() {
    bool dataPresent = false;
    // Use extreme values so that any data will update these.
    minX = std::numeric_limits<double>::max();
    maxX = std::numeric_limits<double>::lowest();
    minY = std::numeric_limits<double>::max();
    maxY = std::numeric_limits<double>::lowest();

    // Process series 1.
    for (size_t i = 0; i < s1x.size(); i++) {
      minX = std::min(minX, s1x[i]);
      maxX = std::max(maxX, s1x[i]);
      minY = std::min(minY, s1y[i]);
      maxY = std::max(maxY, s1y[i]);
      dataPresent = true;
    }

    // Process series 2.
    for (size_t i = 0; i < s2x.size(); i++) {
      minX = std::min(minX, s2x[i]);
      maxX = std::max(maxX, s2x[i]);
      minY = std::min(minY, s2y[i]);
      maxY = std::max(maxY, s2y[i]);
      dataPresent = true;
    }

    // Process series 3.
    for (size_t i = 0; i < s3x.size(); i++) {
      minX = std::min(minX, s3x[i]);
      maxX = std::max(maxX, s3x[i]);
      minY = std::min(minY, s3y[i]);
      maxY = std::max(maxY, s3y[i]);
      dataPresent = true;
    }

    // If no data has been added, set a default range.
    if (!dataPresent) {
      minX = 0; maxX = 100;
      minY = 0; maxY = 100;
    }

    // Avoid division by zero if all points are the same.
    if (minX == maxX) { maxX = minX + 1.0; }
    if (minY == maxY) { maxY = minY + 1.0; }
  }

  // Convert an X value from data coordinates to screen coordinates.
  int dataToScreenX(double dataX) {
    int plotWidth = screenWidth - marginLeft - marginRight;
    double normalized = (dataX - minX) / (maxX - minX);
    return marginLeft + static_cast<int>(normalized * plotWidth);
  }

  // Convert a Y value from data coordinates to screen coordinates.
  // (Note: Screen Y increases downward.)
  int dataToScreenY(double dataY) {
    int plotHeight = screenHeight - marginTop - marginBottom;
    double normalized = (dataY - minY) / (maxY - minY);
    return (screenHeight - marginBottom) - static_cast<int>(normalized * plotHeight);
  }

  // Draw one of the data series as lines connecting points with small markers.
  void drawSeries(const std::vector<double>& xs, const std::vector<double>& ys, vex::color color) {
    if (xs.empty() || ys.empty()) return;
    for (size_t i = 0; i < xs.size(); i++) {
      int xPixel = dataToScreenX(xs[i]);
      int yPixel = dataToScreenY(ys[i]);
      // Draw a small circle for the data point.
      Brain.Screen.drawCircle(xPixel, yPixel, 3, color);
      // If there is a previous point, draw a line connecting them.
      Brain.Screen.setPenColor(color);
      if (i > 0) {
        int xPrev = dataToScreenX(xs[i - 1]);
        int yPrev = dataToScreenY(ys[i - 1]);
        Brain.Screen.drawLine(xPrev, yPrev, xPixel, yPixel);
      }
    }
  }
};
