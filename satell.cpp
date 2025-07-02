#include <iostream>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <thread>
#include <windows.h>
#include <GL/freeglut.h>

const double PI = 3.14159265358979323846;
const double GM = 398600.4418; // Earth's gravitational constant (km^3/s^2)
const double EarthRadius = 6378.135; // Earth's mean radius (km)

double degreesToRadians(double degrees) {
    return degrees * (PI / 180.0);
}

void calculateSatellitePosition(double inclination, double raan, double eccentricity,
                                double argumentOfPerigee, double meanAnomaly, double meanMotion,
                                int year, int dayOfYear, int secondsSinceEpoch,
                                double &x, double &y, double &z) {
    double a = pow((GM / pow(meanMotion, 2.0)), 1.0 / 3.0);
    double M = degreesToRadians(meanAnomaly + meanMotion * secondsSinceEpoch);
    double E = M;
    double tolerance = 1e-8;
    do {
        double E_prev = E;
        E = M + eccentricity * sin(E);
        if (std::abs(E - E_prev) < tolerance) {
            break;
        }
    } while (true);
    double r = a * (1 - eccentricity * cos(E));
    double xOrbital = r * (cos(E) - eccentricity);
    double yOrbital = r * sin(E);
    x = xOrbital * (cos(raan) * cos(argumentOfPerigee) - sin(raan) * sin(argumentOfPerigee * cos(inclination)));
    y = xOrbital * (cos(raan) * sin(argumentOfPerigee) + sin(raan) * cos(argumentOfPerigee) * cos(inclination));
    z = xOrbital * (sin(raan) * cos(inclination));
    double rotationAngle = (360.985647 * (secondsSinceEpoch / 86400.0));
    double xTemp = x * cos(degreesToRadians(rotationAngle)) - y * sin(degreesToRadians(rotationAngle));
    double yTemp = x * sin(degreesToRadians(rotationAngle)) + y * cos(degreesToRadians(rotationAngle));
    x = xTemp;
    y = yTemp;
}

void displayBorder(int width) {
    std::cout << std::string(width, '*') << std::endl;
}

// Global variables to store satellite positions
double satelliteX = 0.0;
double satelliteY = 0.0;
double satelliteZ = 0.0;

// Function to update satellite position
void updateSatellitePosition() {
    double inclination = 51.6436;
    double raan = 343.1856;
    double eccentricity = 0.0007876;
    double argumentOfPerigee = 166.8693;
    double meanAnomaly = 359.5988;
    double meanMotion = 15.54063707;
    int year = 2023;
    int dayOfYear = 10;
    int secondsSinceEpoch = 0;

    while (true) {
        calculateSatellitePosition(inclination, raan, eccentricity, argumentOfPerigee, meanAnomaly, meanMotion, year, dayOfYear, secondsSinceEpoch, satelliteX, satelliteY, satelliteZ);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        secondsSinceEpoch++;
    }
}

// Function to display the satellite position in a window
void displaySatellitePosition() {
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Satellite Position");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1000.0, 1000.0, -1000.0, 1000.0, -1000.0, 1000.0);

    glutDisplayFunc([]() {
        glClear(GL_COLOR_BUFFER_BIT);
        glPointSize(5.0);
        glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0);  // Red color
        glVertex3f(satelliteX, satelliteY, satelliteZ);
        glEnd();
        glFlush();
    });

    glutMainLoop();
}

int main() {
    std::thread positionUpdater(updateSatellitePosition);
    std::thread displayUpdater(displaySatellitePosition);

    positionUpdater.join();
    displayUpdater.join();

    return 0;
}
