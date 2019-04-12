import { browser, by, element, promise } from 'protractor';

export class AppPage {
  navigateTo(): promise.Promise<any> { // tslint:disable-line
    return browser.get('/');
  }

  getParagraphText(): promise.Promise<string> {
    return element(by.css('tp-root h1')).getText();
  }
}
