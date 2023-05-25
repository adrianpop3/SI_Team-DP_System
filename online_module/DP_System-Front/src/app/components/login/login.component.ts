import { Component } from '@angular/core';
import { Router } from '@angular/router';
import { User } from 'src/app/entities/user';
import { LoginService } from 'src/app/services/login.service';
import { UserService } from 'src/app/services/user.service';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent {
  user: User = new User();

  constructor(private loginService: LoginService, private router: Router, private userService: UserService) { }

  userLogin() {
    this.loginService.loginUser(this.user)
      .subscribe(data => {
        this.userService.setCurrentUser(this.user);
        this.loginService.setLoggedIn(true);
        this.router.navigate(['/home']);
      }, 
      error => alert("Login failed!"));
  }
}
