import { Component } from '@angular/core';
import { Router } from '@angular/router';
import { User } from 'src/app/entities/user';
import { RegistrationService } from 'src/app/services/registration.service';

@Component({
  selector: 'app-registration',
  templateUrl: './registration.component.html',
  styleUrls: ['./registration.component.css']
})
export class RegistrationComponent {
  user: User = new User();

  constructor(private registerService: RegistrationService, private router: Router) { }

  // userRegister() {
  //   this.registerService.registerUser(this.user)
  //     .subscribe(data => {
  //       this.router.navigate(['/login']);
  //   },  error=>alert("Sorry, registration failed!"));
  // }
}
