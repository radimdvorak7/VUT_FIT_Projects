describe('Manage Customers', () => {

    it('Not selected', () => {
        // @Given
        cy.visit('http://localhost:8080/');

        cy.get('.backend-link').click();
    
        cy.get('#username').type('admin');
        cy.get('#password').type('admin123');

        cy.get('#login').click();
        cy.wait(1000);
        // @When
        cy.get(':nth-child(2) > .nav-link').click();

        // @Then
        cy.get('#first-name').should('have.value', '');
        cy.get('#last-name').should('have.value', '');
        cy.get('#email').should('have.value', '');
        cy.get('#phone-number').should('have.value', '');
    });

    it('Select customer', () => {
        // @Given
        cy.visit('http://localhost:8080/');

        cy.get('.backend-link').click();
    
        cy.get('#username').type('admin');
        cy.get('#password').type('admin123');

        cy.get('#login').click();
        cy.wait(1000);
        cy.get(':nth-child(2) > .nav-link').click();
        cy.wait(1000);

        // @When
        cy.findCustomer('Gordon Freeman').click();
        
        // @Then
        cy.get('#first-name').should('have.value', 'Gordon');
        cy.get('#last-name').should('have.value', 'Freeman');
        cy.get('#email').should('have.value', 'gordon@testco.local');
        cy.get('#phone-number').should('have.value', '+1 (000) 000-0000');
    });

    it('Add customer', () => {
        // @Given
        cy.visit('http://localhost:8080/');

        cy.get('.backend-link').click();
    
        cy.get('#username').type('admin');
        cy.get('#password').type('admin123');

        cy.get('#login').click();

        cy.wait(1000);
        cy.get(':nth-child(2) > .nav-link').click();
        cy.wait(1000);

        // @When
        cy.get('#add-customer').click();
        cy.addCustomer('Radim', 'Dvořák');
    
        // @Then
        cy.get('.toast-body')
            .invoke('text')
            .should('contains', 'Customer saved successfully.');

        cy.get('#first-name').should('have.value', 'Radim');
        cy.get('#last-name').should('have.value', 'Dvořák');
        cy.get('#email').should('have.value', 'example@gg.com');
        cy.get('#phone-number').should('have.value', '544456154');

        cy.findCustomer('Radim Dvořák').should('exist');

        cy.deleteCustomer('Radim Dvořák');
    });

    it('Edit customer', () => {
        // @Given
        cy.visit('http://localhost:8080/');

        cy.get('.backend-link').click();
    
        cy.get('#username').type('admin');
        cy.get('#password').type('admin123');

        cy.get('#login').click();

        cy.wait(1000);
        cy.get(':nth-child(2) > .nav-link').click();
        cy.wait(1000);

        cy.get('#add-customer').click();
        cy.addCustomer('Radim', 'Dvořák');

        // @When
        cy.findCustomer('Radim Dvořák').click();
        cy.get('#edit-customer').click();
        cy.get('#phone-number').clear().type('544664224');

        // @Then
        cy.get('#phone-number').should('have.value', '544664224');

        cy.get('#save-customer').click();

        cy.get('.toast-body')
            .invoke('text')
            .should('contains', 'Customer saved successfully.');

        cy.deleteCustomer('Radim Dvořák');
    });

    it('Delete customer', () => {
        // @Given
        cy.visit('http://localhost:8080/');

        cy.get('.backend-link').click();
    
        cy.get('#username').type('admin');
        cy.get('#password').type('admin123');

        cy.get('#login').click();

        cy.wait(1000);
        cy.get(':nth-child(2) > .nav-link').click();
        cy.wait(1000);

        cy.get('#add-customer').click();
        cy.addCustomer('Radim', 'Dvořák');

        // @When
        cy.findCustomer('Radim Dvořák').click();
        cy.get('#delete-customer').click();
        cy.get('.modal-footer > .btn-primary').click();

        // @Then
        cy.get('.toast-body')
            .invoke('text')
            .should('contains', 'Customer deleted successfully.');
        cy.findCustomer('Radim Dvořák').should('not.exist');
    });

    it('Search customer - exists', () => {
        // @Given
        cy.visit('http://localhost:8080/');

        cy.get('.backend-link').click();
    
        cy.get('#username').type('admin');
        cy.get('#password').type('admin123');

        cy.get('#login').click();

        cy.wait(1000);
        cy.get(':nth-child(2) > .nav-link').click();
        cy.wait(1000);


        // @When
        cy.get('.customer-row').contains('Gor').click();
        // @Then
        cy.findCustomer('Gor').should('exist');
    });
    
    it('Search customer - doesn\'t exist', () => {
        // @Given
        cy.visit('http://localhost:8080/');

        cy.get('.backend-link').click();
    
        cy.get('#username').type('admin');
        cy.get('#password').type('admin123');

        cy.get('#login').click();

        cy.wait(1000);
        cy.get(':nth-child(2) > .nav-link').click();
        cy.wait(1000);

        // @When
        cy.get('.customer-row').contains('Something').should('not.exist');
        // @Then
        cy.findCustomer('Some').should('not.exist');
    });
});