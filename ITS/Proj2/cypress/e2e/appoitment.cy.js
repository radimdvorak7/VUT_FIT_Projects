describe('Create appointment', () => {

  it('Select date and time', () => {
    // @Given
    cy.visit('http://localhost:8080/');

    cy.get('#wizard-frame-2 > .frame-container > .frame-title')
    .should('be.visible')
    .invoke('text')
    .should('match', /Appointment Date & Time/);

    // @When
    cy.get('.today').click();

    // @And
    cy.get('#available-hours > :nth-child(1)').click();
    
    // @And
    cy.get('#button-next-2 > .svg-inline--fa').click();

    // @Then
    cy.get('#wizard-frame-2 > .frame-container > .frame-title')
    .should('not.be.visible');

    cy.get('#wizard-frame-3 > .frame-container > .frame-title')
    .invoke('text')
    .should('match', /Customer Information/);
  })

  it('Fill personal information - Without filling required fields', () => {
    // @Given
    cy.visit('http://localhost:8080/');
    cy.get('#wizard-frame-2 > .frame-container > .frame-title')
    .should('be.visible')
    .invoke('text')
    .should('match', /Appointment Date & Time/);

    cy.get('.today').click();

    cy.get('#available-hours > :nth-child(1)').click();

    cy.get('#button-next-2 > .svg-inline--fa').click();

    cy.get('#wizard-frame-2 > .frame-container > .frame-title')
    .should('not.be.visible');

    cy.get('#wizard-frame-3 > .frame-container > .frame-title')
    .invoke('text')
    .should('match', /Customer Information/);

    // @And
    cy.get('#first-name').should('have.value', '');
    cy.get('#last-name').should('have.value', '');
    cy.get('#email').should('have.value', '');
    cy.get('#phone-number').should('have.value', '');

    // @When
    cy.get('#button-next-3 > .svg-inline--fa').click();

    // @Then
    cy.get('#first-name').should('have.class', 'required form-control is-invalid');
    cy.get('#last-name').should('have.class', 'required form-control is-invalid');
    cy.get('#email').should('have.class', 'required form-control is-invalid');
    cy.get('#phone-number').should('have.class', 'required form-control is-invalid');
  })

  it('Fill personal information - Valid', () => {
    // @Given
    cy.visit('http://localhost:8080/');
    cy.get('#wizard-frame-2 > .frame-container > .frame-title')
    .should('be.visible')
    .invoke('text')
    .should('match', /Appointment Date & Time/);

    cy.get('.today').click();

    cy.get('#available-hours > :nth-child(1)').click();

    cy.get('#button-next-2 > .svg-inline--fa').click();

    cy.get('#wizard-frame-2 > .frame-container > .frame-title')
    .should('not.be.visible');

    cy.get('#wizard-frame-3 > .frame-container > .frame-title')
    .invoke('text')
    .should('match', /Customer Information/);

    // @When
    cy.get('#first-name').type('Radim');
    cy.get('#last-name').type('Dvořák');
    cy.get('#email').type('example@gg.com');
    cy.get('#phone-number').type('544456154');

    // @And
    cy.get('#button-next-3 > .svg-inline--fa').click();

    // @Then
    cy.get('#wizard-frame-4 > .frame-container > .frame-title')
    .invoke('text')
    .should('match', /Appointment Confirmation/);
  })

  it('Confirmation of appointment', () => {
    // @Given
    cy.visit('http://localhost:8080/');
    cy.get('#wizard-frame-2 > .frame-container > .frame-title')
    .should('be.visible')
    .invoke('text')
    .should('match', /Appointment Date & Time/);

    cy.get('.today').click();

    cy.get('#available-hours > :nth-child(1)').click();

    cy.get('#button-next-2 > .svg-inline--fa').click();

    cy.get('#wizard-frame-2 > .frame-container > .frame-title')
    .should('not.be.visible');

    cy.get('#wizard-frame-3 > .frame-container > .frame-title')
    .invoke('text')
    .should('match', /Customer Information/);

    // @When
    cy.get('#first-name').type('Radim');
    cy.get('#last-name').type('Dvořák');
    cy.get('#email').type('example@gg.com');
    cy.get('#phone-number').type('544456154');

    // @And
    cy.get('#button-next-3 > .svg-inline--fa').click();

    // @Then
    cy.get('#wizard-frame-4 > .frame-container > .frame-title')
    .invoke('text')
    .should('match', /Appointment Confirmation/);
    cy.get('#book-appointment-submit').click();
    cy.url().should('match', /\/index\.php\/booking_confirmation\/of\/[a-zA-Z0-9]+$/)

    // Teardown
    cy.visit('http://localhost:8080/');

    cy.get('.backend-link').click();
    
    cy.get('#username').type('admin');
    cy.get('#password').type('admin123');

    cy.get('#login').click();
    cy.wait(1000);

    cy.get('.fc-timegrid-event-harness > .fc-event').click();

    cy.get('.delete-popover').click();
    cy.get('#message-modal > .modal-dialog > .modal-content > .modal-footer > .btn-primary').click();

    cy.get(':nth-child(2) > .nav-link').click();
    cy.wait(1000);

    cy.deleteCustomer('Radim Dvořák');

  })
})