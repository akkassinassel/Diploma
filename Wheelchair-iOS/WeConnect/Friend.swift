import UIKit
import Contacts

class Friend {
  let firstName: String
  let lastName: String
  let workEmail: String
  var identifier: String?
  let profilePicture: UIImage?
  var storedContact: CNMutableContact?
  var phoneNumberField: (CNLabeledValue<CNPhoneNumber>)?
  
  init(firstName: String, lastName: String, workEmail: String, profilePicture: UIImage?){
    self.firstName = firstName
    self.lastName = lastName
    self.workEmail = workEmail
    self.profilePicture = profilePicture
  }
  
  static func defaultContacts() -> [Friend] {
    return [
      Friend(firstName: "Fire", lastName: "Department", workEmail: "sos@example.com", profilePicture: UIImage(named: "1")),
      Friend(firstName: "Police", lastName: "Department", workEmail: "police@example.com", profilePicture: UIImage(named: "2")),
      Friend(firstName: "Ambulance", lastName: "", workEmail: "ambulance@example.com", profilePicture: UIImage(named: "3")),
      Friend(firstName: "Medical", lastName: "Call Centre", workEmail: "call@example.com", profilePicture: UIImage(named: "4"))]
  }
}

extension Friend: Equatable {
  static func ==(lhs: Friend, rhs: Friend) -> Bool{
    return lhs.firstName == rhs.firstName &&
      lhs.lastName == rhs.lastName &&
      lhs.workEmail == rhs.workEmail &&
      lhs.profilePicture == rhs.profilePicture
  }
}

extension Friend {
  var contactValue: CNContact {
    let contact = CNMutableContact()
    contact.givenName = firstName
    contact.familyName = lastName
    contact.emailAddresses = [CNLabeledValue(label: CNLabelWork, value: workEmail as NSString)]
    if let profilePicture = profilePicture {
      let imageData = profilePicture.jpegData(compressionQuality: 1)
      contact.imageData = imageData
    }
    if let phoneNumberField = phoneNumberField {
      contact.phoneNumbers.append(phoneNumberField)
    }
    return contact.copy() as! CNContact
  }
  
  convenience init?(contact: CNContact) {
    guard let email = contact.emailAddresses.first else { return nil }
    let firstName = contact.givenName
    let lastName = contact.familyName
    let workEmail = email.value as String
    var profilePicture: UIImage?
    if let imageData = contact.imageData {
      profilePicture = UIImage(data: imageData)
    }
    self.init(firstName: firstName, lastName: lastName, workEmail: workEmail, profilePicture: profilePicture)
    if let contactPhone = contact.phoneNumbers.first {
      phoneNumberField = contactPhone
    }
  }
}
