import UIKit
import ContactsUI

class FriendsViewController: UITableViewController {
  var friendsList = Friend.defaultContacts()
  
  override func viewDidLoad() {
    super.viewDidLoad()
    navigationItem.titleView = UIImageView(image: UIImage(named: "RWConnectTitle")!)
  }
  
  override func viewWillAppear(_ animated: Bool) {
    super.viewWillAppear(animated)
    navigationController?.navigationBar.tintColor = .white
  }

  override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
    if
      segue.identifier == "EditFriendSegue",
      // 1
      let cell = sender as? FriendCell,
      let indexPath = tableView.indexPath(for: cell),
      let editViewController = segue.destination as? EditFriendTableViewController {
        let friend = friendsList[indexPath.row]
        // 2
        let store = CNContactStore()
        // 3
        let predicate = CNContact.predicateForContacts(matchingEmailAddress: friend.workEmail)
        // 4
        let keys = [CNContactPhoneNumbersKey as CNKeyDescriptor]
        // 5
        if
          let contacts = try? store.unifiedContacts(matching: predicate, keysToFetch: keys),
          let contact = contacts.first,
          let contactPhone = contact.phoneNumbers.first {
          // 6
            friend.storedContact = contact.mutableCopy() as? CNMutableContact
            friend.phoneNumberField = contactPhone
            friend.identifier = contact.identifier
        }
        editViewController.friend = friend
    }
  }
  
  @IBAction private func addFriends(sender: UIBarButtonItem) {
    // 1
    let contactPicker = CNContactPickerViewController()
    contactPicker.delegate = self
    // 2
    contactPicker.predicateForEnablingContact = NSPredicate(format: "emailAddresses.@count > 0")
    present(contactPicker, animated: true)
  }
  
}

//MARK: - UITableViewDataSource
extension FriendsViewController {
  override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
    return friendsList.count
  }
  
  override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
    let cell = tableView.dequeueReusableCell(withIdentifier: "FriendCell", for: indexPath)
    
    if let cell = cell as? FriendCell {
      let friend = friendsList[indexPath.row]
      cell.friend = friend
    }
    
    return cell
  }
}

//MARK: - UITableViewDelegate
extension FriendsViewController {
  override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
    tableView.deselectRow(at: indexPath, animated: true)
    // 1
    let friend = friendsList[indexPath.row]
    let contact = friend.contactValue
    // 2
    let contactViewController = CNContactViewController(forUnknownContact: contact)
    contactViewController.hidesBottomBarWhenPushed = true
    contactViewController.allowsEditing = false
    contactViewController.allowsActions = false
    // 3
    navigationController?.navigationBar.tintColor = .appBlue
    navigationController?.pushViewController(contactViewController, animated: true)
  }
}

//MARK: - CNContactPickerDelegate
extension FriendsViewController: CNContactPickerDelegate {
  func contactPicker(_ picker: CNContactPickerViewController,
                     didSelect contacts: [CNContact]) {
    let newFriends = contacts.compactMap { Friend(contact: $0) }
    for friend in newFriends {
      if !friendsList.contains(friend) {
        friendsList.append(friend)
      }
    }
    tableView.reloadData()
  }
}
