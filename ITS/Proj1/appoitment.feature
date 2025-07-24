Feature: Create appointment

Background: 
    Given customer is not logged in

# Coverage matrix col.: 1
Scenario: Select date and time
    Given web browser is at "Appointment Date & Time"
    When customer selects date
    And customer selects time
    And customer clicks on "Next"
    Then customer is shown page "Customer Information"

# Coverage matrix col.: 2
Scenario Outline: Fill personal information - Without filling required fields
    Given web browser is at page "Customer Information"
    And <required> fields are empty 
    When customer clicks on "Next"
    Then warnings next to <required> fields are shown
    Examples:
        | required        |
        | First Name      |
        | Last Name       |
        | Email           | 
        | Phone Number    | 

# Coverage matrix col.: 3
Scenario: Fill personal information - Valid 
    Given web browser is at "Customer Information"
    And required fields are filled
    When customer clicks on "Next"
    Then the customer is shown page "Appointment Confirmation"

# Coverage matrix col.: 4
Scenario: Confirmation of appointment
    Given web browser is at "Appointment Confirmation"
    When customer clicks on "Confirm"
    Then customer gets message "Your appointment has been successfully registered!"