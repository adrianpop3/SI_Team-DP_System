import { Component } from '@angular/core';
import { Router } from '@angular/router';
import { User } from 'src/app/entities/user';
import { LoginService } from 'src/app/services/login.service';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent {
  user: User = new User();

  constructor(private loginService: LoginService, private router: Router) { }

  userLogin() {
    this.loginService.loginUser(this.user)
      .subscribe(data => {
        this.router.navigate(['/home']);
      }, error => alert("Login failed!"));
  }
}
