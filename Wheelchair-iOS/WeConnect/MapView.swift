
import UIKit
import MapKit

class MapView: UIViewController {
  @IBOutlet private var mapView: MKMapView!
  
    override func viewDidLoad() {
        super.viewDidLoad()
      
      let initialLocation = CLLocation(latitude: 43.238949, longitude: 76.889709)
      mapView.centerToLocation(initialLocation)

    }
  }
private extension MKMapView {
  func centerToLocation(
    _ location: CLLocation,
    regionRadius: CLLocationDistance = 1000
  ) {
    let coordinateRegion = MKCoordinateRegion(
      center: location.coordinate,
      latitudinalMeters: regionRadius,
      longitudinalMeters: regionRadius)
    setRegion(coordinateRegion, animated: true)
  }
}
