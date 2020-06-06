import UIKit
import Contacts

class FriendCell: UITableViewCell {
  @IBOutlet private weak var contactNameLabel: UILabel!
  @IBOutlet private weak var contactEmailLabel: UILabel!
  @IBOutlet private weak var contactImageView: UIImageView! {
    didSet {
      contactImageView.layer.masksToBounds = true
      contactImageView.layer.cornerRadius = 22.0
    }
  }
  
  var friend : Friend? {
    didSet {
      configureCell()
    }
  }
  
  private func configureCell() {
    let formatter = CNContactFormatter()
    formatter.style = .fullName
    guard let friend = friend,
      let name = formatter.string(from: friend.contactValue) else { return }
    contactNameLabel.text = name
    contactEmailLabel.text = friend.workEmail
    contactImageView.image = friend.profilePicture ?? UIImage(named: "PlaceholderProfilePic")
  }
}
