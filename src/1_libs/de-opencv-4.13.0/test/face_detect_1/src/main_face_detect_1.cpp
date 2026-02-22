#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main() {

    std::cout << "Suche verfügbare Kameras..." << std::endl;

    for (int i = 0; i < 10; i++) {
        cv::VideoCapture cap(i);
        if (cap.isOpened()) {
            std::cout << "Kamera gefunden: ID = " << i << std::endl;
            cap.release();
        }
    }

    std::vector<int> backends = {
        cv::CAP_ANY,
        cv::CAP_V4L2,
        cv::CAP_MSMF,
        cv::CAP_DSHOW,
        cv::CAP_AVFOUNDATION
    };

    cv::VideoCapture cam;

    for (int b : backends) {
        std::cout << "Versuche Backend: " << b << std::endl;
        cam.open(0, b);
        if (cam.isOpened()) {
            std::cout << "Kamera erfolgreich geöffnet mit Backend " << b << std::endl;
            break;
        }
    }

    if (!cam.isOpened()) {
        std::cerr << "Keine Kamera konnte geöffnet werden!" << std::endl;
        return -1;
    }

    std::cout << "Backend: " << cam.getBackendName() << std::endl;

#if 0
            cv::Mat frame;
            while (true) {
                cap >> frame;
                if (frame.empty()) break;

                cv::imshow("Kamera", frame);
                if (cv::waitKey(1) == 27) break;
            }

#endif
    // Kamera öffnen (0 = Default-Kamera)
    cv::VideoCapture cap(1, cv::CAP_DSHOW);
    if (!cap.isOpened()) {
        std::cerr << "Kamera konnte nicht geöffnet werden!" << std::endl;
        return -1;
    }


    // Haar-Cascade für Gesichtserkennung laden
    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load("media/OpenCV/data/haarcascades/haarcascade_frontalface_default.xml")) {
        std::cerr << "Fehler beim Laden der Haar-Cascade!" << std::endl;
        return -1;
    }

    cv::Mat frame, gray;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // In Graustufen konvertieren
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);

        // Gesichter erkennen
        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.1, 4, 0, cv::Size(30, 30));

        // Jedes Gesicht markieren
        for (size_t i = 0; i < faces.size(); i++) {
            cv::Scalar color(
                rand() % 256,
                rand() % 256,
                rand() % 256
            );

            cv::rectangle(frame, faces[i], color, 2);

            std::string label = "Face " + std::to_string(i + 1);
            cv::putText(frame, label,
                        cv::Point(faces[i].x, faces[i].y - 10),
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, color, 2);
        }

        // Bild anzeigen
        cv::imshow("Gesichtserkennung", frame);

        // Mit ESC beenden
        if (cv::waitKey(1) == 27) break;
    }

    return 0;
}
