import UIKit
import Contacts

extension UIColor {
  static let appBlue = UIColor(red: 128.0 / 255.0, green: 128.0 / 255.0, blue: 128.0 / 255.0, alpha: 1)
}

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {
	var window: UIWindow?
	
	func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
		window?.backgroundColor = .white
		
		UINavigationBar.appearance().barStyle = .default
		UINavigationBar.appearance().barTintColor = .appBlue
		UINavigationBar.appearance().tintColor = .white
    UINavigationBar.appearance().isOpaque = true
    UINavigationBar.appearance().titleTextAttributes = [.foregroundColor: UIColor.white]
		UITabBar.appearance().tintColor = .appBlue
    
    CNContactStore().requestAccess(for: .contacts) { (access, error) in
      print("Access: \(access)")
    }
    
		return true
	}
}
