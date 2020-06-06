import UIKit
import Contacts

class EditFriendTableViewController: UITableViewController {
  @IBOutlet private weak var nameLabel: UILabel!
  @IBOutlet private weak var emailLabel: UILabel!
  @IBOutlet private weak var phoneTypeLabel: UILabel!
  @IBOutlet private weak var phoneTextField: UITextField!
  @IBOutlet private weak var saveButton: UIButton!

  var friend: Friend?

  override func viewDidLoad() {
    super.viewDidLoad()
    setup()
  }

  private func setup() {
    guard let friend = friend else {
      nameLabel.text = ""
      emailLabel.text = ""
      phoneTextField.text = ""
      phoneTextField.isEnabled = false
      return
    }
    let formatter = CNContactFormatter()
    formatter.style = .fullName
    if let name = formatter.string(from: friend.contactValue) {
      nameLabel.text = name
    } else {
      nameLabel.text = "Name Not Available"
    }
    emailLabel.text = friend.workEmail
    if let phoneNumberField = friend.phoneNumberField,
      let label = phoneNumberField.label {
      phoneTypeLabel.text = CNLabeledValue<CNPhoneNumber>.localizedString(forLabel: label)
      phoneTextField.text = phoneNumberField.value.stringValue
    }
  }

  @IBAction private func save(_ sender: Any) {
    phoneTextField.resignFirstResponder()
    let store = CNContactStore()
    guard let friend = friend,
      let phoneNumberText = phoneTextField.text else { return }
    let phoneNumberValue = CNPhoneNumber(stringValue: phoneNumberText)
    let saveRequest = CNSaveRequest()
    
    if let storedContact = friend.storedContact,
      let phoneNumberToEdit = storedContact.phoneNumbers.first(
        where: { $0 == friend.phoneNumberField }
      ),
      let index = storedContact.phoneNumbers.firstIndex(of: phoneNumberToEdit) {
      
      // 1
      let newPhoneNumberField = phoneNumberToEdit.settingValue(phoneNumberValue)
      storedContact.phoneNumbers.remove(at: index)
      storedContact.phoneNumbers.insert(newPhoneNumberField, at: index)
      friend.phoneNumberField = newPhoneNumberField
      // 2
      saveRequest.update(storedContact)
      friend.storedContact = nil
    } else if let unsavedContact = friend.contactValue.mutableCopy() as? CNMutableContact {
      // 3
      let phoneNumberField = CNLabeledValue(label: CNLabelPhoneNumberMain,
                                            value: phoneNumberValue)
      unsavedContact.phoneNumbers = [phoneNumberField]
      friend.phoneNumberField = phoneNumberField
      // 4
      saveRequest.add(unsavedContact, toContainerWithIdentifier: nil)
    }

    do {
      try store.execute(saveRequest)
      let controller = UIAlertController(title: "Success",
                                         message: nil,
                                         preferredStyle: .alert)
      controller.addAction(UIAlertAction(title: "OK", style: .default))
      present(controller, animated: true)
      setup()
    } catch {
      print(error)
    }
  }
}
