import { Component } from '@angular/core';
import { PwaService } from './services/pwa-service.service';
import { TranslateService } from '@ngx-translate/core';

@Component({
  selector: 'tp-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {
  constructor(public pwa: PwaService,
              private readonly translate: TranslateService) {
    this.translate.setDefaultLang('en');
    const browserLang = this.translate.getBrowserLang();
    this.translate.use(browserLang);
  }
}
